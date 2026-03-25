// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/CGameplayTypes.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Cooldown, "ability.combo.cooldown", "cooldown tag that will be added to the combo ability cooldown effect")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo, "ability.combo", "tag for the combo ability itself")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Damage, "ability.combo.damage", "tag for sending damage detection event to the combo ability")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change, "ability.combo.change", "tag to tell combo ability what combo is up for next")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change_Combo01, "ability.combo.change.combo01", "tag to tell combo ability next combo is combo01")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change_Combo02, "ability.combo.change.combo02", "tag to tell combo ability next combo is combo02")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change_Combo03, "ability.combo.change.combo03", "tag to tell combo ability next combo is combo03")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change_Combo04, "ability.combo.change.combo04", "tag to tell combo ability next combo is combo04")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Combo_Change_End, "ability.combo.change.end", "tag to tell combo ability no more combo next after this point")

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STAT_Dead, "stat.dead", "tag added when player is dead")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STAT_Stun, "stat.stun", "a character will be stunned when this tag is added to them")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STAT_Invisible, "stat.invisible", "tag added to make character not be perceived by AI")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STAT_Aiming, "stat.aiming", "tag added to change the character control to the aiming mode")