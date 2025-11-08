// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerChar.generated.h"

UCLASS()
class GAM312V2_API APlayerChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Linked UFUNCTIONS TO INPUTS CNRT . to make into definition
	UFUNCTION()
		void MoveForward(float axisValue);

	UFUNCTION()
		void MoveRight(float axisValue);

	UFUNCTION()
		void StartJump();

	UFUNCTION()
		void StopJump();

	UFUNCTION()
		void Findobject();

	UPROPERTY(VisibleAnywhere) //sets camera to visible
		UCameraComponent* PlayerCamComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats") //editanywhere allows you to change it from anywhere //bluepring readwrite lets you edit is
		float Health = 100.0f; //0.0f for float tag //catagogy is set via settings // sets health float//Sets in player defaults

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float Hunger = 100.0f; //sets hunger float//Sets in player defaults

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float Stamina = 100.0f; //sets stamina float//Sets in player defaults

	UPROPERTY(EditAnywhere, Category = "Resources") //Setting int for resources
		int Wood;

	UPROPERTY(EditAnywhere, Category = "Resources") //Setting int for resources
		int Stone;

	UPROPERTY(EditAnywhere, Category = "Resources") //Setting int for resources
		int Berry;

	UPROPERTY(EditAnywhere, Category = "HitMarker") //creating hit marker
		UMaterialInterface* hitDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		TArray<int> ResourcesArray;

	UPROPERTY(EditAnywhere, Category = "Resources") //save names is array
		TArray<FString> ResourceNameArray;

	UFUNCTION(BlueprintCallable) //creates and calls this function
		void SetHealth(float amount);

	UFUNCTION(BlueprintCallable) //creates and calls this function
		void SetHunger(float amount);

	UFUNCTION(BlueprintCallable) //creates and calls this function
		void SetStamina(float amount);

	UFUNCTION()
		void DecreaseStats();

	UFUNCTION()
		void GiveResources(float amount, FString resourceType);

};
