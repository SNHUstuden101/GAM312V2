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


	BuildingArray.SetNum(3);//Sets Arrays
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

	playerUI->UpdateBars(Health, Hunger, Stamina); //sets tick updates for the bars

	if (isBuilding)
	{
		if (spawnedPart)//cheacks to see if its a valid object
		{
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f; //Sets a pint 400 unit infront of player to set actors location
			FVector EndLocation = StartLocation + Direction;
			spawnedPart->SetActorLocation(EndLocation); 
		}
	}

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
	PlayerInputComponent->BindAction("RotPart", IE_Released, this, &APlayerChar::RotateBuilding);//Rotates the building binding e
}
//Handle forward movement input
void APlayerChar::MoveForward(float AxisValue)
{
	//Get the forward direction based on the controller's rotation
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	//Applys Movement input
	AddMovementInput(Direction, AxisValue);
}
//Handle right movement input
void APlayerChar::MoveRight(float AxisValue)
{
	//Get the right direction based on the controller's rotation
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	//Apply movement input 
	AddMovementInput(Direction, AxisValue);
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
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f; //800 units ahead of camera
	FVector EndLocation = StartLocation + Direction; 

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); //ignore player character
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;
	
	if (!isBuilding) //Disables is isbuilding is false
	{
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams)) //set line trace
		{
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());//cast to resource

			if (Stamina > 5.0f) //checks to see if stamina is greater than 5

				if (HitResource)
				{
					FString HitName = HitResource->resourceName; //getting resource value
					int ResourceValue = HitResource->resourceAmount; //checking resourse ammount

					HitResource->totalResource = HitResource->totalResource - ResourceValue; //subtracts value from total amount when hit

					if (HitResource->totalResource > ResourceValue) //if theres resource left we set the value
					{
						GiveResources(ResourceValue, HitName); //sets new value and name

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

	else
	{
		isBuilding = false; //sets is building to false
	}


}

void APlayerChar::SetHealth(float Amount)
{
	if (Health + Amount < 100) //sets add to or subtract from Health
	{
		Health = Health + Amount;

	}
}

void APlayerChar::SetHunger(float Amount)
{
	if (Hunger + Amount < 100) //sets add to or subtract from Hunger
	{
		Hunger = Hunger + Amount;

	}
}

void APlayerChar::SetStamina(float Amount)
{
	if (Stamina + Amount < 100) //sets add to or subtract from stamina
	{
		Stamina = Stamina + Amount;

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

void APlayerChar::GiveResources(float Amount, FString ResourceType)
{
	if (ResourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + Amount; //adds amount to resourse array
	}

	if (ResourceType == "Stone")
	{
		ResourcesArray[1] = ResourcesArray[1] + Amount;
	}

	if (ResourceType == "Berry")
	{
		ResourcesArray[2] = ResourcesArray[2] + Amount;
	}

}

void APlayerChar::UpdateResources(float WoodAmount, float StoneAmount, FString BuildingObject)
{
	if (WoodAmount <= ResourcesArray[0])
	{
		if (StoneAmount <= ResourcesArray[1])
		{
			ResourcesArray[0] = ResourcesArray[0] - WoodAmount; //Subtracts the amount to build object
			ResourcesArray[1] = ResourcesArray[1] - StoneAmount;

			if (BuildingObject == "Wall")
			{
				BuildingArray[0] = BuildingArray[0] + 1; //Adds to the built object
			}

			if (BuildingObject == "Floor")
			{
				BuildingArray[1] = BuildingArray[1] + 1;//Adds to the built object
			}

			if (BuildingObject == "Ceiling")
			{
				BuildingArray[2] = BuildingArray[2] + 1;//Adds to the built object
			}
		}
	}
}

void APlayerChar::SpawnBuilding(int BuildingId, bool& IsSuccess)
{
	if (!isBuilding)
	{
		if (BuildingArray[BuildingId] >= 1) //check to see if you have atleast 1
		{
			isBuilding = true; //sets it true
			FActorSpawnParameters SpawnParams;
			FVector StartLocation = PlayerCamComp->GetComponentLocation(); //Checking direction again
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector Endlocation = StartLocation* Direction;
			FRotator MyRot(0, 0, 0);

			BuildingArray[BuildingId] = BuildingArray[BuildingId] - 1; //Subtracts part from array

			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, Endlocation, MyRot, SpawnParams); //spawns part at location

			IsSuccess = true; //Checks wether sucess is true or not
		}

		IsSuccess = false; //Checks wether sucess is true or not

	}

}

void APlayerChar::RotateBuilding()
{
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0)); //roates object 90 degrees at pivot
	}
}

