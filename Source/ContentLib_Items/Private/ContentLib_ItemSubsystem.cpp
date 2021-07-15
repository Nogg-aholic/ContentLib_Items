#include "ContentLib_ItemSubsystem.h"


#include "FGItemCategory.h"
#include "Resources/FGItemDescriptorNuclearFuel.h"
#include "Resources/FGResourceDescriptor.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Unlocks/FGUnlockRecipe.h"


FContentLib_ItemVisualKit::FContentLib_ItemVisualKit()
{
	FluidColor = FColor();
	GasColor = FColor();
}

UTexture2D* FContentLib_ItemVisualKit::GetSmallIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *SmallIcon);
}

UTexture2D* FContentLib_ItemVisualKit::GetBigIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *BigIcon);
}

UStaticMesh* FContentLib_ItemVisualKit::GetMesh() const
{
	return LoadObject<UStaticMesh>(nullptr, *Mesh);
}

TSharedRef<FJsonObject> FContentLib_ItemVisualKit::GetAsJsonObject(const TSubclassOf<UFGItemDescriptor> Item)
{
	const auto CDO = Cast<UFGItemDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Mesh = MakeShared<FJsonValueString>(CDO->mConveyorMesh->GetPathName());
	const auto BigIcon = MakeShared<FJsonValueString>(CDO->mPersistentBigIcon->GetPathName());
	const auto SmallIcon = MakeShared<FJsonValueString>(CDO->mSmallIcon->GetPathName());
	auto Color = MakeShared<FJsonObject>();
	const auto FluidColor_R = MakeShared<FJsonValueNumber>(CDO->mFluidColor.R);
	const auto FluidColor_G = MakeShared<FJsonValueNumber>(CDO->mFluidColor.G);
	const auto FluidColor_B = MakeShared<FJsonValueNumber>(CDO->mFluidColor.B);
	const auto FluidColor_A = MakeShared<FJsonValueNumber>(CDO->mFluidColor.A);
	Color->Values.Add("r", FluidColor_R);
	Color->Values.Add("g", FluidColor_G);
	Color->Values.Add("b", FluidColor_B);
	Color->Values.Add("a", FluidColor_A);

	auto ColorGas = MakeShared<FJsonObject>();
	const auto GasColor_R = MakeShared<FJsonValueNumber>(CDO->mGasColor.R);
	const auto GasColor_G = MakeShared<FJsonValueNumber>(CDO->mGasColor.G);
	const auto GasColor_B = MakeShared<FJsonValueNumber>(CDO->mGasColor.B);
	const auto GasColor_A = MakeShared<FJsonValueNumber>(CDO->mGasColor.A);
	ColorGas->Values.Add("r", GasColor_R);
	ColorGas->Values.Add("g", GasColor_G);
	ColorGas->Values.Add("b", GasColor_B);
	ColorGas->Values.Add("a", GasColor_A);

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("GasColor", MakeShared<FJsonValueObject>(ColorGas));

	return Obj;
}

FString FContentLib_ItemVisualKit::GenerateFromClass(const TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
		wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(GetAsJsonObject(Item), JsonWriter);
	return Write;
}

FContentLib_ItemVisualKit FContentLib_ItemVisualKit::GenerateFromString(FString String)
{
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
	{
		if (String == "")
		UE_LOG(LogTemp, Error, TEXT("Empty String  %s"), *String)
		else if (!String.StartsWith("{"))
		UE_LOG(LogTemp, Error, TEXT("String doesnt start with '{' %s"), *String)
		else if (!String.EndsWith("}"))
		UE_LOG(LogTemp, Error, TEXT("String doesnt end with '}'  %s"), *String);

		return FContentLib_ItemVisualKit();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_ItemVisualKit();
	}

	FContentLib_ItemVisualKit Item;
	if (Result->HasField("Mesh") && Result->TryGetField("Mesh")->Type == EJson::String)
	{
		Item.Mesh = *Result->TryGetField("Mesh")->AsString();
	}
	if (Result->HasField("BigIcon") && Result->TryGetField("BigIcon")->Type == EJson::String)
	{
		Item.BigIcon = *Result->TryGetField("BigIcon")->AsString();
	}
	if (Result->HasField("SmallIcon") && Result->TryGetField("SmallIcon")->Type == EJson::String)
	{
		Item.SmallIcon = *Result->TryGetField("SmallIcon")->AsString();
	}
	if (Result->HasField("FluidColor") && Result->TryGetField("FluidColor")->Type == EJson::Object)
	{
		const auto Obj = Result->TryGetField("FluidColor")->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a"))
		{
			if (Result->TryGetField("r")->Type == EJson::Number)
				Item.FluidColor.R = Result->TryGetField("r")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("R value in Color is not of type Number !"));
			}
			if (Result->TryGetField("g")->Type == EJson::Number)
				Item.FluidColor.G = Result->TryGetField("g")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Result->TryGetField("b")->Type == EJson::Number)
				Item.FluidColor.B = Result->TryGetField("b")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Result->TryGetField("a")->Type == EJson::Number)
				Item.FluidColor.R = Result->TryGetField("a")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
	}
	if (Result->HasField("GasColor") && Result->TryGetField("GasColor")->Type == EJson::Object)
	{
		const auto Obj = Result->TryGetField("GasColor")->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a"))
		{
			if (Result->TryGetField("r")->Type == EJson::Number)
				Item.GasColor.R = Result->TryGetField("r")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("R value in Color is not of type Number !"));
			}
			if (Result->TryGetField("g")->Type == EJson::Number)
				Item.GasColor.G = Result->TryGetField("g")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Result->TryGetField("b")->Type == EJson::Number)
				Item.GasColor.B = Result->TryGetField("b")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Result->TryGetField("a")->Type == EJson::Number)
				Item.GasColor.A = Result->TryGetField("a")->AsNumber();
			else
			{
				UE_LOG(LogTemp, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
	}
	return Item;
}

void FContentLib_ItemVisualKit::ApplyFromStruct(const TSubclassOf<UFGItemDescriptor> Item) const
{
	auto* Obj = Item.GetDefaultObject();
	if (GetMesh())
		Obj->mConveyorMesh = GetMesh();

	if (GetBigIcon())
		Obj->mPersistentBigIcon = GetBigIcon();

	if (GetSmallIcon())
		Obj->mSmallIcon = GetSmallIcon();

	if (FluidColor != FColor(0, 0, 0, 0))
	{
		Obj->mFluidColor = FluidColor;
	}

	if (GasColor != FColor(0, 0, 0, 0))
	{
		Obj->mGasColor = GasColor;
	}
}

FContentLib_ResourceItem::FContentLib_ResourceItem(): CollectSpeedMultiplier(-1)
{
	PingColor = FColor(0, 0, 0, 0);
}

TSharedRef<FJsonObject> FContentLib_ResourceItem::GetAsJsonObject(TSubclassOf<UFGResourceDescriptor> Item)
{
	const auto CDO = Cast<UFGResourceDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	auto Color = MakeShared<FJsonObject>();
	const auto PingColor_R = MakeShared<FJsonValueNumber>(CDO->mPingColor.R);
	const auto PingColor_G = MakeShared<FJsonValueNumber>(CDO->mPingColor.G);
	const auto PingColor_B = MakeShared<FJsonValueNumber>(CDO->mPingColor.B);
	const auto PingColor_A = MakeShared<FJsonValueNumber>(CDO->mPingColor.A);
	Color->Values.Add("r", PingColor_R);
	Color->Values.Add("g", PingColor_G);
	Color->Values.Add("b", PingColor_B);
	Color->Values.Add("a", PingColor_A);
	const auto CollectSpeedMultiplier = MakeShared<FJsonValueNumber>(CDO->mCollectSpeedMultiplier);

	Obj->Values.Add("PingColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("CollectSpeedMultiplier", CollectSpeedMultiplier);

	return Obj;
}

FString FContentLib_ResourceItem::GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		TSubclassOf<UFGResourceDescriptor> Resource = Item;
		FString Write;
		const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
			wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(GetAsJsonObject(Resource), JsonWriter);
		return Write;
	}
	else
		return "";
}

FContentLib_NuclearFuelItem::FContentLib_NuclearFuelItem(): AmountOfWaste(-1)
{
}

TSharedRef<FJsonObject> FContentLib_NuclearFuelItem::GetAsJsonObject(TSubclassOf<UFGItemDescriptorNuclearFuel> Item)
{
	const auto CDO = Cast<UFGItemDescriptorNuclearFuel>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();

	const auto SpentFuelClass = MakeShared<FJsonValueString>(CDO->GetSpentFuelClass(Item)->GetPathName());

	const auto AmountOfWaste = MakeShared<FJsonValueNumber>(CDO->GetAmountWasteCreated(Item));

	Obj->Values.Add("SpentFuelClass", SpentFuelClass);
	Obj->Values.Add("AmountOfWaste", AmountOfWaste);
	return Obj;
}

FString FContentLib_NuclearFuelItem::GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		TSubclassOf<UFGItemDescriptorNuclearFuel> Resource = Item;
		FString Write;
		const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
			wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(GetAsJsonObject(Resource), JsonWriter);
		return Write;
	}
	else
		return "";
}

FContentLib_Item::FContentLib_Item(): Form(EResourceForm::RF_INVALID), EnergyValue(-1), RadioactiveDecay(-1),
                                      CanBeDiscarded(-1), RememberPickUp(-1),
                                      ResourceSinkPoints(-1),
                                      FuelWasteItem()
{
}


FString FContentLib_Item::GenerateFromClass(const TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	const auto CDO = Cast<UFGItemDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto NameShort = MakeShared<FJsonValueString>(CDO->mAbbreviatedDisplayName.ToString());
	const auto Description = MakeShared<FJsonValueString>(CDO->mDescription.ToString());
	const auto ItemCategory = MakeShared<FJsonValueString>(CDO->mItemCategory->GetPathName());

	const auto VisualKit = MakeShared<FJsonValueObject>(FContentLib_ItemVisualKit::GetAsJsonObject(Item));
	const auto EnergyValue = MakeShared<FJsonValueNumber>(CDO->mEnergyValue);
	const auto RadioactiveDecay = MakeShared<FJsonValueNumber>(CDO->mRadioactiveDecay);
	const auto CanBeDiscarded = MakeShared<FJsonValueBoolean>(CDO->mCanBeDiscarded);
	const auto RememberPickUp = MakeShared<FJsonValueBoolean>(CDO->mRememberPickUp);
	const auto ResourceSinkPoints = MakeShared<FJsonValueNumber>(CDO->mResourceSinkPoints);
	Obj->Values.Add("Name", Name);
	Obj->Values.Add("NameShort", NameShort);
	Obj->Values.Add("Description", Description);
	Obj->Values.Add("Category", ItemCategory);
	Obj->Values.Add("VisualKit", VisualKit);
	Obj->Values.Add("EnergyValue", EnergyValue);
	Obj->Values.Add("RadioactiveDecay", RadioactiveDecay);
	Obj->Values.Add("CanBeDiscarded", CanBeDiscarded);
	Obj->Values.Add("RememberPickUp", RememberPickUp);
	Obj->Values.Add("ResourceSinkPoints", ResourceSinkPoints);

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		TSubclassOf<UFGResourceDescriptor> Resource = Item;
		const auto ResourceItem = MakeShared<FJsonValueObject>(FContentLib_ResourceItem::GetAsJsonObject(Resource));
		Obj->Values.Add("ResourceItem", ResourceItem);
	}

	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		TSubclassOf<UFGItemDescriptorNuclearFuel> Resource = Item;
		const auto FuelWasteItem = MakeShared<FJsonValueObject>(FContentLib_NuclearFuelItem::GetAsJsonObject(Resource));
		Obj->Values.Add("FuelWasteItem", FuelWasteItem);
	}

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
		wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FContentLib_Item FContentLib_Item::GenerateFromString(const FString String)
{
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_Item();

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
		return FContentLib_Item();

	auto Item = FContentLib_Item();
	if (Result->HasField("Form") && Result->TryGetField("Form")->Type == EJson::String)
	{
		const FString CS = Result->TryGetField("Form")->AsString();
		if (CS.Equals("Solid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_SOLID;
		}
		else if (CS.Equals("Liquid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_LIQUID;
		}
		else if (CS.Equals("Gas", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_GAS;
		}
		else if (CS.Equals("Heat", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_HEAT;
		}
	}

	if (Result->HasField("Name") && Result->TryGetField("Name")->Type == EJson::String)
	{
		Item.Name = Result->TryGetField("Name")->AsString();
	}
	if (Result->HasField("NameShort") && Result->TryGetField("NameShort")->Type == EJson::String)
	{
		Item.NameShort = Result->TryGetField("NameShort")->AsString();
	}
	if (Result->HasField("Description") && Result->TryGetField("Description")->Type == EJson::String)
	{
		Item.Description = Result->TryGetField("Description")->AsString();
	}
	if (Result->HasField("Category") && Result->TryGetField("Category")->Type == EJson::String)
	{
		Item.Category = Result->TryGetField("Category")->AsString();
	}
	if (Result->HasField("VisualKit") && Result->TryGetField("VisualKit")->Type == EJson::String)
	{
		Item.VisualKit = Result->TryGetField("VisualKit")->AsString();
	}
	if (Result->HasField("EnergyValue") && Result->TryGetField("EnergyValue")->Type == EJson::Number)
	{
		Item.EnergyValue = Result->TryGetField("EnergyValue")->AsNumber();
	}
	if (Result->HasField("RadioactiveDecay") && Result->TryGetField("RadioactiveDecay")->Type == EJson::Number)
	{
		Item.RadioactiveDecay = Result->TryGetField("RadioactiveDecay")->AsNumber();
	}
	if (Result->HasField("CanBeDiscarded") && Result->TryGetField("CanBeDiscarded")->Type == EJson::Boolean)
	{
		Item.CanBeDiscarded = static_cast<int32>(Result->TryGetField("CanBeDiscarded")->AsBool());
	}
	if (Result->HasField("RememberPickUp") && Result->TryGetField("RememberPickUp")->Type == EJson::Boolean)
	{
		Item.RememberPickUp = static_cast<int32>(Result->TryGetField("RememberPickUp")->AsBool());
	}

	if (Result->HasField("ResourceSinkPoints") && Result->TryGetField("ResourceSinkPoints")->Type == EJson::Number)
	{
		Item.ResourceSinkPoints = Result->TryGetField("ResourceSinkPoints")->AsNumber();
	}

	if (Result->HasField("ResourceItem") && Result->TryGetField("ResourceItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("ResourceItem")->AsObject();

		if (OBJ->HasField("PingColor") && OBJ->TryGetField("PingColor")->Type == EJson::Object)
		{
			const auto Obj = OBJ->TryGetField("PingColor")->AsObject();
			if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a"))
			{
				if (Obj->TryGetField("r")->Type == EJson::Number)
					Item.ResourceItem.PingColor.R = Obj->TryGetField("r")->AsNumber();
				else
				{
					UE_LOG(LogTemp, Error, TEXT("R value in Color is not of type Number !"));
				}
				if (Obj->TryGetField("g")->Type == EJson::Number)
					Item.ResourceItem.PingColor.G = Obj->TryGetField("g")->AsNumber();
				else
				{
					UE_LOG(LogTemp, Error, TEXT("G value in Color is not of type Number !"));
				}

				if (Obj->TryGetField("b")->Type == EJson::Number)
					Item.ResourceItem.PingColor.B = Obj->TryGetField("b")->AsNumber();
				else
				{
					UE_LOG(LogTemp, Error, TEXT("b value in Color is not of type Number !"));
				}

				if (Obj->TryGetField("a")->Type == EJson::Number)
					Item.ResourceItem.PingColor.A = Obj->TryGetField("a")->AsNumber();
				else
				{
					UE_LOG(LogTemp, Error, TEXT("A value in Color is not of type Number !"));
				}
			}
		}
		if (OBJ->HasField("CollectSpeedMultiplier") && OBJ->TryGetField("CollectSpeedMultiplier")->Type == EJson::Number
		)
		{
			Item.ResourceItem.CollectSpeedMultiplier = OBJ->TryGetField("CollectSpeedMultiplier")->AsNumber();
		}
	}

	if (Result->HasField("FuelWasteItem") && Result->TryGetField("FuelWasteItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("FuelWasteItem")->AsObject();

		if (OBJ->HasField("SpentFuelClass") && OBJ->TryGetField("SpentFuelClass")->Type == EJson::String)
		{
			Item.FuelWasteItem.SpentFuelClass = OBJ->TryGetField("SpentFuelClass")->AsString();
		}
		if (OBJ->HasField("AmountOfWaste") && OBJ->TryGetField("AmountOfWaste")->Type == EJson::Number)
		{
			Item.FuelWasteItem.AmountOfWaste = OBJ->TryGetField("AmountOfWaste")->AsNumber();
		}
	}
	return Item;
}

void FContentLib_Item::ApplyFromStruct(TSubclassOf<UFGItemDescriptor> Item, UContentLib_ItemSubsystem* Subsystem)
{
	UFGItemDescriptor* CDO = Item.GetDefaultObject();

	if (Form != EResourceForm::RF_INVALID)
	{
		CDO->mForm = Form;
	}
	if (Name != "")
	{
		CDO->mDisplayName = FText::FromString(Name);
		CDO->mUseDisplayNameAndDescription = true;
	}
	if (NameShort != "")
	{
		CDO->mAbbreviatedDisplayName = FText::FromString(NameShort);
	}
	if (Description != "")
	{
		CDO->mDescription = FText::FromString(Description);
	}
	if (Category != "")
	{
		if (Category.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *Category);
			if (Loaded && Loaded->IsChildOf(UFGItemCategory::StaticClass()))
				CDO->mItemCategory = Loaded;
			else
			{
				FString Left;
				FString Right;
				Category.Split("/", &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
				if (Right != "")
				{
					FString TempName = "Cat_";
					TempName.Append(Right);
					UFGItemCategory* Cat = Cast<UFGItemCategory>(
						Subsystem->CreateContentLibItem(TempName, UFGItemCategory::StaticClass()));
					if (Cat)
					{
						Cat->mDisplayName = FText::FromString(Right);
						CDO->mItemCategory = Cat->GetClass();
					}
					else
					{
						UClass * Find = FindObject<UClass>(ANY_PACKAGE, *TempName, false);
						if(!Find)
						{
							Find =  FindObject<UClass>(ANY_PACKAGE, *TempName.Append("_C"), false);
						}
						if(Find && Find->IsChildOf(UFGItemCategory::StaticClass()))
						{
							Cat->mDisplayName = FText::FromString(Right);
							CDO->mItemCategory = Find;
						}
					}
				}
			}
		}
		else
		{
			bool Added = false;
			for (auto e : Subsystem->ItemCategories)
			{
				FString S = e->GetName();
				FString A = Category;
				FString DescPre = FString("Cat_").Append(Category);
				if (!DescPre.EndsWith("_C"))
				{
					DescPre.Append("_C");
				}
				if (S.Equals(Category, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase)
					|| S.Equals(DescPre, ESearchCase::IgnoreCase))
				{
					CDO->mItemCategory = e;
					Added = true;
					break;
				}
			}
			if (!Added)
			{
				UFGItemCategory* Cat = Cast<UFGItemCategory>(
					Subsystem->CreateContentLibItem(Category, UFGItemCategory::StaticClass()));
				if (Cat)
				{
					Cat->mDisplayName = FText::FromString(Category);
					CDO->mItemCategory = Cat->GetClass();
				}
				else
				{
					UClass * Find = FindObject<UClass>(ANY_PACKAGE, *Category, false);
					if(!Find)
					{
						Find =  FindObject<UClass>(ANY_PACKAGE, *Category.Append("_C"), false);
					}
					if(Find && Find->IsChildOf(UFGItemCategory::StaticClass()))
					{
						Cat->mDisplayName = FText::FromString(Category);
						CDO->mItemCategory = Find;
					}
				}
			}
		}
	}
	if (VisualKit != "")
	{
		if (Subsystem->ImportedVisualKits.Contains(VisualKit))
			Subsystem->ImportedVisualKits.Find(VisualKit)->ApplyFromStruct(Item);
		else if (Subsystem->VisualKits.Contains(VisualKit))
			Subsystem->VisualKits.Find(VisualKit)->ApplyFromStruct(Item);
	}

	if (EnergyValue != -1)
	{
		CDO->mEnergyValue = EnergyValue;
	}

	if (RadioactiveDecay != -1)
	{
		CDO->mRadioactiveDecay = RadioactiveDecay;
	}

	if (CanBeDiscarded != -1)
	{
		CDO->mCanBeDiscarded = static_cast<bool>(CanBeDiscarded);
	}

	if (RememberPickUp != -1)
	{
		CDO->mRememberPickUp = static_cast<bool>(RememberPickUp);
	}

	if (ResourceSinkPoints != -1)
	{
		CDO->mResourceSinkPoints = ResourceSinkPoints;
	}

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		if (ResourceItem.PingColor != FColor(0, 0, 0, 0))
			Cast<UFGResourceDescriptor>(CDO)->mPingColor = ResourceItem.PingColor;
		if (ResourceItem.CollectSpeedMultiplier != -1)
		{
			Cast<UFGResourceDescriptor>(CDO)->mCollectSpeedMultiplier = ResourceItem.CollectSpeedMultiplier;
		}
	}
	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		if (FuelWasteItem.SpentFuelClass != "")
		{
			if (FuelWasteItem.SpentFuelClass.Contains("/"))
			{
				UClass* Loaded = LoadObject<UClass>(nullptr, *FuelWasteItem.SpentFuelClass);
				if (Loaded && Loaded->IsChildOf(UFGItemCategory::StaticClass()))
					Cast<UFGItemDescriptorNuclearFuel>(CDO)->mSpentFuelClass = Loaded;
			}
			else
			{
				for (auto e : Subsystem->Items)
				{
					FString S = e->GetName();
					FString A = FuelWasteItem.SpentFuelClass;
					FString DescPre = FString("Desc").Append(FuelWasteItem.SpentFuelClass);
					if (!DescPre.EndsWith("_C"))
					{
						DescPre.Append("_C");
					}
					if (S.Equals(FuelWasteItem.SpentFuelClass, ESearchCase::IgnoreCase) || S.Equals(
						A.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
					{
						Cast<UFGItemDescriptorNuclearFuel>(CDO)->mSpentFuelClass = e;
						break;
					}
				}
			}
		}
		if (FuelWasteItem.AmountOfWaste != -1)
		{
			Cast<UFGItemDescriptorNuclearFuel>(CDO)->mAmountOfWaste = FuelWasteItem.AmountOfWaste;
		}
	}
}

void UContentLib_ItemSubsystem::FillLoadedClasses()
{
}

void UContentLib_ItemSubsystem::CollectVisualKits()
{
	GetDerivedClasses(UFGItemDescriptor::StaticClass(), Items, true);
	GetDerivedClasses(UFGItemCategory::StaticClass(), ItemCategories, true);
	for (auto i : Items)
	{
		UFGItemDescriptor* Item = Cast<UFGItemDescriptor>(i->GetDefaultObject());;
		FContentLib_ItemVisualKit Kit;
		Kit.Mesh = Item->mConveyorMesh->GetPathName();
		Kit.BigIcon = Item->mPersistentBigIcon->GetPathName();
		Kit.SmallIcon = Item->mSmallIcon->GetPathName();
		Kit.FluidColor = Item->mFluidColor;
		Kit.GasColor = Item->mGasColor;
		VisualKits.Add(i->GetName(), Kit);
	}
}

bool UContentLib_ItemSubsystem::FindVisualKit(const FString Name, FContentLib_ItemVisualKit& Kit)
{
	for (const auto i : ImportedVisualKits)
	{
		FString S = Name;
		FString Tp = i.Key;
		FString DescPre = FString("Desc_").Append(Tp);
		if (!Tp.EndsWith("_C"))
		{
			DescPre.Append("_C");
		}
		if (S.Equals(Tp, ESearchCase::IgnoreCase) || S.Equals(Tp.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
		{
			Kit = i.Value;
			return true;
		}
	}
	for (const auto i : VisualKits)
	{
		FString S = Name;
		FString Tp = i.Key;
		FString DescPre = FString("Desc_").Append(Tp);
		if (!Tp.EndsWith("_C"))
		{
			DescPre.Append("_C");
		}
		if (S.Equals(Tp, ESearchCase::IgnoreCase) || S.Equals(Tp.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
		{
			Kit = i.Value;
			return true;
		}
	}
	Kit = FContentLib_ItemVisualKit();
	return false;
}

FString UContentLib_ItemSubsystem::GenerateFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	return FContentLib_Item::GenerateFromClass(Item);
}

FContentLib_Item UContentLib_ItemSubsystem::GenerateFromString(FString String)
{
	return FContentLib_Item::GenerateFromString(String);
}

FContentLib_ItemVisualKit UContentLib_ItemSubsystem::GenerateKitFromString(FString String)
{
	return FContentLib_ItemVisualKit::GenerateFromString(String);
}

FString UContentLib_ItemSubsystem::GetAsString(FContentLib_ItemVisualKit Kit)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Mesh = MakeShared<FJsonValueString>(Kit.Mesh);
	const auto BigIcon = MakeShared<FJsonValueString>(Kit.BigIcon);
	const auto SmallIcon = MakeShared<FJsonValueString>(Kit.SmallIcon);
	auto Color = MakeShared<FJsonObject>();
	const auto FluidColor_R = MakeShared<FJsonValueNumber>(Kit.FluidColor.R);
	const auto FluidColor_G = MakeShared<FJsonValueNumber>(Kit.FluidColor.G);
	const auto FluidColor_B = MakeShared<FJsonValueNumber>(Kit.FluidColor.B);
	const auto FluidColor_A = MakeShared<FJsonValueNumber>(Kit.FluidColor.A);
	Color->Values.Add("r", FluidColor_R);
	Color->Values.Add("g", FluidColor_G);
	Color->Values.Add("b", FluidColor_B);
	Color->Values.Add("a", FluidColor_A);

	auto ColorGas = MakeShared<FJsonObject>();
	const auto GasColor_R = MakeShared<FJsonValueNumber>(Kit.GasColor.R);
	const auto GasColor_G = MakeShared<FJsonValueNumber>(Kit.GasColor.G);
	const auto GasColor_B = MakeShared<FJsonValueNumber>(Kit.GasColor.B);
	const auto GasColor_A = MakeShared<FJsonValueNumber>(Kit.GasColor.A);
	ColorGas->Values.Add("r", GasColor_R);
	ColorGas->Values.Add("g", GasColor_G);
	ColorGas->Values.Add("b", GasColor_B);
	ColorGas->Values.Add("a", GasColor_A);

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("GasColor", MakeShared<FJsonValueObject>(ColorGas));
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
		wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

void UContentLib_ItemSubsystem::ApplyFromStruct(const TSubclassOf<UFGItemDescriptor> Item, FContentLib_Item Struct)
{
	Struct.ApplyFromStruct(Item, this);
}


TSubclassOf<UObject> UContentLib_ItemSubsystem::CreateContentLibItem(FString Name, UClass* ParentClass)
{
	if (Name == "" || FindObject<UClass>(ANY_PACKAGE, *Name, false) || FindObject<UClass>(
		ANY_PACKAGE, *Name.Append("_C"), false))
		return nullptr;
	const EClassFlags ParamsClassFlags = CLASS_Native | CLASS_MatchedSerializers;
	//Code below is taken from GetPrivateStaticClassBody
	//Allocate memory from ObjectAllocator for class object and call class constructor directly
	UClass* ConstructedClassObject = (UClass*)GUObjectAllocator.AllocateUObject(
		sizeof(UDynamicClass), alignof(UDynamicClass), true);
	::new(ConstructedClassObject)UDynamicClass(
		EC_StaticConstructor,
		*Name,
		ParentClass->GetStructureSize(),
		ParentClass->GetMinAlignment(),
		CLASS_Intrinsic,
		CASTCLASS_None,
		UObject::StaticConfigName(),
		EObjectFlags(RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet),
		ParentClass->ClassConstructor,
		ParentClass->ClassVTableHelperCtorCaller,
		ParentClass->ClassAddReferencedObjects, nullptr);

	//Set super structure and ClassWithin (they are required prior to registering)
	FCppClassTypeInfoStatic TypeInfoStatic = {false};
	ConstructedClassObject->SetSuperStruct(ParentClass);
	ConstructedClassObject->ClassWithin = UObject::StaticClass();
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#if WITH_EDITOR
	//Field with cpp type info only exists in editor, in shipping SetCppTypeInfoStatic is empty
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#endif
	//Register pending object, apply class flags, set static type info and link it
	ConstructedClassObject->RegisterDependencies();

	ConstructedClassObject->DeferredRegister(UDynamicClass::StaticClass(), TEXT("/ContentLib_Items/"), *Name);

	//Mark class as Constructed and perform linking
	ConstructedClassObject->ClassFlags |= (EClassFlags)(ParamsClassFlags | CLASS_Constructed);
	ConstructedClassObject->AssembleReferenceTokenStream(true);
	ConstructedClassObject->StaticLink();

	//Make sure default class object is initialized
	ConstructedClassObject->GetDefaultObject();
	return ConstructedClassObject;
}
