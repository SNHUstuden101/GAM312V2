// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"

// Sets default values
APlayerChar::APlayerChar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Initial component of camera setup
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));
	//links camera to head
	PlayerCamComp->SetupAttachment(GetMesh(), "head");
	//links rotation
	PlayerCamComp->bUsePawnControlRotation = true;

	ResourcesArray.SetNum(3);
	ResourceNameArray.Add(TEXT("Wood"));
	ResourceNameArray.Add(TEXT("Stone"));
	ResourceNameArray.Add(TEXT("Berry"));

}
// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	
	Super::BeginPlay();

	FTimerHandle StatsTimerHandle; //set handle for timer
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerChar::DecreaseStats, 2.0f, true); //this references self. 2.0f is the amount of seconds it calls the function
	
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Bind axis inputs for movement and camera control
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);
	//Bind action inputs for jumping
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::Findobject);//allows interact when pressed
}
//Handle forward movement input
void APlayerChar::MoveForward(float axisValue)
{
	//Get the forward direction based on the controller's rotation
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	//Applys Movement input
	AddMovementInput(Direction, axisValue);
}
//Handle right movement input
void APlayerChar::MoveRight(float axisValue)
{
	//Get the right direction based on the controller's rotation
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	//Apply movement input 
	AddMovementInput(Direction, axisValue);
}

//Start jumping when the jump button is pressed
void APlayerChar::StartJump()
{
	bPressedJump = true; //sets true
}
//Stop jumping when the jump button is released
void APlayerChar::StopJump()
{
	bPressedJump = false; //sets false
}
//currently empty code but will be use later
void APlayerChar::Findobject()
{
	//
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f; //800 units ahead of camera
	FVector EndLocation = StartLocation + Direction; 

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); //ignore player character
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams)) //set line trace
	{
		AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());//cast to resource

		if (Stamina > 5.0f) //checks to see if stamina is greater than 5

			if (HitResource)
			{
				FString hitName = HitResource->resourceName; //getting resource value
				int resourceValue = HitResource->resourceAmount; //checking resourse ammount

				HitResource->totalResource = HitResource->totalResource - resourceValue; //subtracts value from total amount when hit

				if (HitResource->totalResource > resourceValue) //if theres resource left we set the value
				{
					GiveResources(resourceValue, hitName); //sets new value and name

					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Collected")); //Shows that we collect resource

					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);

					SetStamina(-5.0f); //subtracts 5 from smanima
				}
				else
				{
					HitResource->Destroy();
					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Depleated")); //destroys and shows resourse depleated
				}
		}
	}

}

void APlayerChar::SetHealth(float amount)
{
	if (Health + amount < 100) //sets add to or subtract from Health
	{
		Health = Health + amount;

	}
}

void APlayerChar::SetHunger(float amount)
{
	if (Hunger + amount < 100) //sets add to or subtract from Hunger
	{
		Hunger = Hunger + amount;

	}
}

void APlayerChar::SetStamina(float amount)
{
	if (Stamina + amount < 100) //sets add to or subtract from stamina
	{
		Stamina = Stamina + amount;

	}
}

void APlayerChar::DecreaseStats()
{
	if (Hunger > 0)
	{
		SetHunger(-1.0f);
	}
	
	SetStamina(10.0f);

	if (Hunger <= 0)
	{
		SetHealth(-3.0f);
	}
}

void APlayerChar::GiveResources(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + amount; //adds amount to resouse array
	}

	if (resourceType == "Stone")
	{
		ResourcesArray[1] = ResourcesArray[1] + amount;
	}

	if (resourceType == "Berry")
	{
		ResourcesArray[2] = ResourcesArray[2] + amount;
	}

}

