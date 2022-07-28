// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Boid.generated.h"

UCLASS()
class ASSN3_API ABoid : public APawn
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ball, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* Detector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AlignmentConstant = 15;//250
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float CohesionConstant = 1; // 10000
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AvoidenceConstant = 25; //39

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this pawn's properties
	ABoid();

	/** Returns Ball subobject **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Ball;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float RollTorque = 50000000.0f;// 50000000.0f

	TSet<AActor*> Neighbors;

	UFUNCTION()
		void OnOverlapBegin(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

	inline void CalculateAlignment(FVector& Alignment, AActor* Boid);
	inline void CalculateCohesion(FVector& Cohesion, AActor* Boid);
	inline void CalculateAvoidence(FVector& Avoidence, AActor* Boid);
	void ApplyCohesionForce(const FVector& CohesionCenter);
	void ApplyAvoidenceForce(FVector& AvoidenceVector);
	void SetAlignment(const FVector& AlignmentVector);
};
