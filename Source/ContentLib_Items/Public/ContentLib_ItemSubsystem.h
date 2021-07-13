

#pragma once

#include "CoreMinimal.h"


#include "Dom/JsonObject.h"
#include "Resources/FGItemDescriptor.h"
#include "Resources/FGItemDescriptorNuclearFuel.h"
#include "Resources/FGResourceDescriptor.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ContentLib_ItemSubsystem.generated.h"

USTRUCT(BlueprintType)
struct  CONTENTLIB_ITEMS_API  FContentLib_ItemVisualKit
{
	GENERATED_BODY()

	
	FContentLib_ItemVisualKit();
	UTexture2D* GetSmallIcon() const;

	UTexture2D* GetBigIcon() const;

	class UStaticMesh* GetMesh() const;
	static TSharedRef<FJsonObject> GetAsJsonObject(TSubclassOf<UFGItemDescriptor> Item);
	static FString GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item);


	static FContentLib_ItemVisualKit GenerateFromString(FString String);
	void ApplyFromStruct(TSubclassOf<UFGItemDescriptor> Item) const;

	UPROPERTY(BlueprintReadWrite)
	FString Mesh;
	
	UPROPERTY(BlueprintReadWrite)
	FString BigIcon;
	
	UPROPERTY(BlueprintReadWrite)
	FString SmallIcon;
	
	UPROPERTY(BlueprintReadWrite)
	FColor FluidColor;
	
	UPROPERTY(BlueprintReadWrite)
	FColor GasColor;

};
USTRUCT(BlueprintType)
struct  CONTENTLIB_ITEMS_API  FContentLib_ResourceItem
{
	GENERATED_BODY()
	FContentLib_ResourceItem();


	static TSharedRef<FJsonObject> GetAsJsonObject(TSubclassOf<UFGResourceDescriptor> Item);
	static FString GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item);


	UPROPERTY( EditDefaultsOnly, Category = "Item|Resource" )
	FLinearColor PingColor;

	UPROPERTY( EditDefaultsOnly, Category = "Item|Resource" )
	float CollectSpeedMultiplier;

	bool Valid;;

};

USTRUCT(BlueprintType)
struct  CONTENTLIB_ITEMS_API  FContentLib_NuclearFuelItem
{
	GENERATED_BODY()
	FContentLib_NuclearFuelItem();
	static TSharedRef<FJsonObject> GetAsJsonObject(TSubclassOf<UFGItemDescriptorNuclearFuel> Item);
	static FString GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item);

	UPROPERTY( EditDefaultsOnly, Category = "Nuclear Fuel" )
	FString SpentFuelClass;

	UPROPERTY( EditDefaultsOnly, Category = "Nuclear Fuel" )
	int32 AmountOfWaste;
};
USTRUCT(BlueprintType)
struct  CONTENTLIB_ITEMS_API  FContentLib_Item
{
	GENERATED_BODY()


	FContentLib_Item();
	friend class UContentLib_ItemSubsystem;

	static FString GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item);
	static FContentLib_Item GenerateFromString(FString String);
	void ApplyFromStruct(TSubclassOf<UFGItemDescriptor> Item, const UContentLib_ItemSubsystem * Subsystem) const;

	
	UPROPERTY(BlueprintReadWrite)
	EResourceForm Form ;
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString NameShort;
	UPROPERTY(BlueprintReadWrite)
	FString Description;
	UPROPERTY(BlueprintReadWrite)
	FString ItemCategory;
	UPROPERTY(BlueprintReadWrite)
	FString VisualKit;
	
	UPROPERTY(BlueprintReadWrite)
	float EnergyValue;
	UPROPERTY(BlueprintReadWrite)
	float RadioactiveDecay;

	UPROPERTY(BlueprintReadWrite)
	int32 CanBeDiscarded;
	UPROPERTY(BlueprintReadWrite)
	int32 RememberPickUp;

	
	UPROPERTY(BlueprintReadWrite)
	int32 ResourceSinkPoints;


	UPROPERTY(BlueprintReadWrite)
	FContentLib_ResourceItem ResourceItem;

	UPROPERTY(BlueprintReadWrite)
	FContentLib_NuclearFuelItem FuelWasteItem;
	
};

/**
 * 
 */
UCLASS()
class CONTENTLIB_ITEMS_API UContentLib_ItemSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
    void FillLoadedClasses();
	
	UFUNCTION(BlueprintCallable)
	void CollectVisualKits();

	UFUNCTION(BlueprintCallable)
	bool FindVisualKit(FString Name, FContentLib_ItemVisualKit & Kit);

	UFUNCTION(BlueprintCallable)
	FString GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item);
	UFUNCTION(BlueprintCallable)
	FContentLib_Item GenerateFromString(FString String);
	
	UFUNCTION(BlueprintCallable)
	FContentLib_ItemVisualKit GenerateKitFromString(FString String);


	UFUNCTION(BlueprintCallable)
	FString GetAsString(FContentLib_ItemVisualKit Kit);
	
	UFUNCTION(BlueprintCallable)
	void ApplyFromStruct(TSubclassOf<UFGItemDescriptor> Item, FContentLib_Item Struct);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UFGItemDescriptor> CreateContentLibItem(FString Name, TSubclassOf<UFGItemDescriptor> Class);


	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Items;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> ItemCategories;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FContentLib_ItemVisualKit> VisualKits;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString,FContentLib_ItemVisualKit> ImportedVisualKits;

	UPROPERTY(BlueprintReadWrite)
	TMap<TSubclassOf<UFGItemDescriptor>,FString> CreatedItems;
	UPROPERTY(BlueprintReadWrite)
	TMap<TSubclassOf<UFGItemDescriptor>,FString> ItemPatches;
};
