// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "assn3ball.h"
#include "GenericPlatform/GenericPlatformMath.h"

#include "DrawDebugHelpers.h"

// Sets default values
ABoid::ABoid()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Rolling/Meshes/BallMesh.BallMesh"));

	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetAngularDamping(0.5f);
	Ball->SetLinearDamping(1);
	Ball->BodyInstance.MassScale = 1;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);

	Ball->SetupAttachment(RootComponent);
	Ball->SetRelativeLocation(FVector::ZeroVector);

	Detector = CreateDefaultSubobject<USphereComponent>(TEXT("Detector"));
	Detector->SetSphereRadius(1000.f);//375 can try bigger

	Detector->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnOverlapBegin);
	Detector->OnComponentEndOverlap.AddDynamic(this, &ABoid::OnOverlapEnd);
	Detector->SetHiddenInGame(true);
	Detector->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> TemporaryNeighbors;
	Detector->GetOverlappingActors(TemporaryNeighbors);
	for (AActor* i : TemporaryNeighbors)
	{
		if (i == this || i == nullptr) continue;
		if (!Cast<ABoid>(i) && !Cast<Aassn3Ball>(i)) continue;
		Neighbors.Add(i);
	}

}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector  AlignmentVector, CohesionVector, AvoidenceVector;
	AlignmentVector = CohesionVector = AvoidenceVector = FVector::ZeroVector;
	if (Neighbors.Num())
	{
		for (AActor* i : Neighbors)
		{
			CalculateAlignment(AlignmentVector, i);
			CalculateCohesion(CohesionVector, i);
			CalculateAvoidence(AvoidenceVector, i);;
		}
		AlignmentVector /= Neighbors.Num();
		CohesionVector /= Neighbors.Num();
		AvoidenceVector /= Neighbors.Num();

		SetAlignment(AlignmentVector);
		ApplyCohesionForce(CohesionVector);
		ApplyAvoidenceForce(AvoidenceVector);
	}

}

// Called to bind functionality to input
void ABoid::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoid::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (Neighbors.Contains(OtherActor) || OtherActor == this) return;
	if (!Cast<ABoid>(OtherActor) && !Cast<Aassn3Ball>(OtherActor)) return;

	Neighbors.Add(OtherActor);

}

void ABoid::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	if (!Neighbors.Contains(OtherActor)) return;
	Neighbors.Remove(OtherActor);
}

void ABoid::CalculateAlignment(FVector& Alignment, AActor* Boid)
{
	if (!Boid) return;
	Alignment += Boid->GetVelocity();
}

void ABoid::CalculateCohesion(FVector& Cohesion, AActor* Boid)
{
	if (!Boid) return;
	Cohesion += Boid->GetActorLocation();
}

void ABoid::CalculateAvoidence(FVector& Avoidence, AActor* Boid)
{
	if (!Boid) return;

	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(
		Boid->GetActorLocation(),
		GetActorLocation()
	);
	float Distance = GetDistanceTo(Boid);
	Direction *= (1000000 * FGenericPlatformMath::Pow(Distance, -3));
	Avoidence += (Direction);
}

void ABoid::ApplyCohesionForce(const FVector& CohesionCenter)
{
	float Distance = FVector::Distance(GetActorLocation(), CohesionCenter);
	FVector CohesionForce = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), CohesionCenter);
	CohesionForce = CohesionForce * CohesionConstant * Distance;
	CohesionForce.Z = 0.f;
	Ball->AddImpulse(CohesionForce);
}

void ABoid::ApplyAvoidenceForce(FVector& CalculatedAvoidenceVector)
{
	Ball->AddImpulse(CalculatedAvoidenceVector * AvoidenceConstant * 5000);
}

void ABoid::SetAlignment(const FVector& AlignmentVector)
{
	FVector AverageVelocity = AlignmentVector - GetVelocity();
	AverageVelocity.Z = 0;

	Ball->AddImpulse(AverageVelocity * AlignmentConstant);
}