#pragma once

// enum followed by class makes it a "Scoped Enum" so that if we have to use its value, we will have to fully use the variable followed by enum name. So its a safety for multiple enums(Like for character and enemy). Also there is another Unreal Engine Convention to write the prefix of enum before each state so user may now whose state is which. 
// Also we used uint8 ( unsigned 8 bits int to only reserve that much state whether enum have 1000 states )
UENUM(BlueprintType) // Used to expose Enums to Blueprints and UMETA() to change their display names for blueprints
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandWeapon UMETA(DisplayName = "EquippedOne-HandedWeapon"), ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwo-HandedWeapon")
};

UENUM( BlueprintType )
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayeName = "Unoccupied") ,
	EAS_Attacking UMETA(DisplayName = "Attacking")
};