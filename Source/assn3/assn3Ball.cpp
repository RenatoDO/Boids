// Copyright Epic Games, Inc. All Rights Reserved.

#include "assn3Ball.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Boid.h"

Aassn3Ball::Aassn3Ball()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Rolling/Meshes/BallMesh.BallMesh"));

	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetAngularDamping(0.1f);
	Ball->SetLinearDamping(0.1f);
	Ball->BodyInstance.MassScale = 3.5f;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	RootComponent = Ball;

	// Create a camera boom attached to the root (ball)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true); // Rotation of the ball should not affect rotation of boom
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 1200.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;

	// Create a camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Set up forces
	RollTorque = 50000000.0f;
	JumpImpulse = 350000.0f;
	bCanJump = true; // Start being able to jump

	Detector = CreateDefaultSubobject<USphereComponent>(TEXT("Detector"));
	Detector->SetupAttachment(RootComponent);
	Detector->SetSphereRadius(375.f);
	Detector->SetGenerateOverlapEvents(true);
	Detector->OnComponentBeginOverlap.AddDynamic(this, &Aassn3Ball::OnOverlapBegin);
	Detector->OnComponentEndOverlap.AddDynamic(this, &Aassn3Ball::OnOverlapEnd);
}

void Aassn3Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (AActor* i : Neighbors)
	{
		//ApplyAvoidenceForce(i);
	}
}

void Aassn3Ball::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAxis("MoveRight", this, &Aassn3Ball::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &Aassn3Ball::MoveForward);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &Aassn3Ball::Jump);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &Aassn3Ball::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &Aassn3Ball::TouchStopped);
}

void Aassn3Ball::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> TemporaryNeighbors;
	Detector->GetOverlappingActors(TemporaryNeighbors);
	for (AActor* i : TemporaryNeighbors)
	{
		if (i == this || i == nullptr) continue;
		if (!Cast<ABoid>(i)) continue;
		Neighbors.Add(i);
	}

}

void Aassn3Ball::MoveRight(float Val)
{
	const FVector Torque = FVector(-1.f * Val * RollTorque, 0.f, 0.f);
	Ball->AddTorqueInRadians(Torque);
}

void Aassn3Ball::MoveForward(float Val)
{
	const FVector Torque = FVector(0.f, Val * RollTorque, 0.f);
	Ball->AddTorqueInRadians(Torque);	
}

void Aassn3Ball::Jump()
{
	//return;
	if(bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}

void Aassn3Ball::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	bCanJump = true;
}

void Aassn3Ball::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}

}

void Aassn3Ball::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}

void Aassn3Ball::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (Neighbors.Contains(OtherActor) || OtherActor == this) return;
	if (!Cast<ABoid>(OtherActor)) return;
	Neighbors.Add(OtherActor);

}

void Aassn3Ball::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!OtherActor) return;
	if (!Neighbors.Contains(OtherActor)) return;
	Neighbors.Remove(OtherActor);

}

void Aassn3Ball::ApplyAvoidenceForce(AActor* Neighbor)
{
	ABoid* CastedBoid = Cast<ABoid>(Neighbor);
	if (CastedBoid == nullptr) return;
	FVector UnitAvoidenceVector = UKismetMathLibrary::GetDirectionUnitVector
	(
		CastedBoid->GetActorLocation(),
		GetActorLocation()
	);

	UnitAvoidenceVector *= -1;

	FVector AvoidenceForce = (UnitAvoidenceVector * 3906250) / FGenericPlatformMath::Pow(GetDistanceTo(CastedBoid), 2);
	CastedBoid->Ball->AddImpulse(AvoidenceForce);

}