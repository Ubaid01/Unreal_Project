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
	EAS_Attacking UMETA(DisplayName = "Attacking") ,
	EAS_EquippingWeapon UMETA( DisplayName = "Equipping Weapon")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayeName = "Alive"),
	EDP_Death1 UMETA(DisplayName = "Death_1"),
	EDP_Death2 UMETA(DisplayName = "Death_2"),
	EDP_Death3 UMETA(DisplayName = "Death_3"),
	EDP_Death4 UMETA(DisplayName = "Death_4"),
	EDP_Death5 UMETA(DisplayName = "Death_5"),
	EDP_Death6 UMETA(DisplayName = "Death_6"),
	EDP_Death7 UMETA(DisplayName = "Death_7")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Looking UMETA( DisplayName = "Looking_Around" ) ,
	EES_Patrolling UMETA( DisplayName = "Patrolling" ) ,
	EES_Chasing UMETA( DisplayName = "Chasing" ) ,
	EES_Attacking UMETA( DisplayName = "Attacking" ) 
};