#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFightStyle : uint8
{
	BareHand = 0,
	Axe = 1,
	AxeMelee = 2,
	Bow = 3,
	Dagger = 4,
	DualSword = 5,
	Katana1 = 6,
	Katana2 = 7,
	Katana3 = 8,
	MagicStaff = 9,
	SwordAndShield = 10,
	SwordNoShield = 11,
	SwordMelee = 12
};

UENUM(BlueprintType)
enum class EBodyType : uint8
{
	Male = 0,
	Female = 1
};

UENUM(BlueprintType)
enum class ERace : uint8
{
	Apache = 0,// super dash ability, replace regular dash with more powerful one
	Yamato = 1,// fly ability, jump for 2nd time while in air will float character
	HomoSuperior = 2,// rage ability, alter M1 while in rage
};