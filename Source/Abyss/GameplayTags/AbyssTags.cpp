// Copyright (c) 2025 Leon Lee


#include "AbyssTags.h"

namespace AbyssTags
{

	//
	namespace CharacterClass
	{
		namespace Hero
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Elementalist, "AbyssTags.CharacterClass.Hero.Elementalist", "Tag for the hero Elementalist Class");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Warrior,"AbyssTags.CharacterClass.Hero.Warrior", "Tag for the hero Warrior Class");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Assassin,"AbyssTags.CharacterClass.Hero.Assassin", "Tag for the hero Assassin Class");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ranger,"AbyssTags.CharacterClass.Hero.Ranger", "Tag for the hero Ranger Class");
		}

		namespace Enemy
		{
			namespace Minions
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Elementalist, "AbyssTags.CharacterClass.Enemy.Minions.Elementalist", "Tag for the Minions Elementalist Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Warrior, "AbyssTags.CharacterClass.Enemy.Minions.Warrior", "Tag for the Minions Warrior Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Assassin, "AbyssTags.CharacterClass.Enemy.Minions.Assassin", "Tag for the Minions Assassin Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ranger, "AbyssTags.CharacterClass.Enemy.Minions.Ranger", "Tag for the Minions Ranger Class");
			}

			namespace Boss
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Elementalist, "AbyssTags.CharacterClass.Enemy.Boss.Elementalist", "Tag for the Boss Elementalist Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Warrior, "AbyssTags.CharacterClass.Enemy.Boss.Warrior", "Tag for the Boss Warrior Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Assassin, "AbyssTags.CharacterClass.Enemy.Boss.Assassin", "Tag for the Boss Assassin Class");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ranger, "AbyssTags.CharacterClass.Enemy.Boss.Ranger", "Tag for the Boss Ranger Class");
			}
		}
	}
	
	namespace SetByCaller
	{
		namespace DamageType
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Physical, "AbyssTags.SetByCaller.DamageType.Physical", "Tag for the Physical DamageType");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "AbyssTags.SetByCaller.DamageType.Fire", "Tag for the Fire DamageType");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ice, "AbyssTags.SetByCaller.DamageType.Ice", "Tag for the Ice DamageType");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "AbyssTags.SetByCaller.DamageType.Lightning", "Tag for the Lightning DamageType");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Poison, "AbyssTags.SetByCaller.DamageType.Poison", "Tag for the Poison DamageType");
		}
		namespace Debuff
		{
			//Debuff类型
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Physical, "AbyssTags.SetByCaller.Debuff.Physical", "Tag for the Physical Debuff");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "AbyssTags.SetByCaller.Debuff.Fire", "Tag for the Fire Debuff");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ice, "AbyssTags.SetByCaller.Debuff.Ice", "Tag for the Ice Debuff");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "AbyssTags.SetByCaller.Debuff.Lightning", "Tag for the Lightning Debuff");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Poison, "AbyssTags.SetByCaller.Debuff.Poison", "Tag for the Poison Debuff");
			
			//Debuff的数值属性
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chance, "AbyssTags.SetByCaller.Debuff.Chance", "Tag for the Debuff Chance");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "AbyssTags.SetByCaller.Debuff.Damage", "Tag for the Debuff Damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Duration, "AbyssTags.SetByCaller.Debuff.Duration", "Tag for the Debuff Duration");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Frequency, "AbyssTags.SetByCaller.Debuff.Frequency", "Tag for the Debuff Frequency");
		}
	}
	
	
	namespace Ability
	{
		
		namespace Melee
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SwordLightAttack, "AbyssTags.Ability.Melee.SwordLightAttack", "Tag for Ability that Melee LightAttack.")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SwordHeavyAttack, "AbyssTags.Ability.Melee.SwordHeavyAttack", "Tag for Ability that Melee HeavyAttack");
		}

		namespace Projectile
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireBalt, "AbyssTags.Ability.Projectile.FireBalt", "Tag for Ability that FireBalt Projectile.")
		}
		namespace Beam
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Electrocute, "AbyssTags.Ability.Beam.Electrocute", "Tag for Ability that Electrocute Beam.")
		}
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "AbyssTags.Ability.ActivateOnGiven", "Tag for Ability that should activate immediately once given.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "AbyssTags.Ability.Death", "Tag for Ability that should be Add when the owner dies.")
		namespace Hero
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "AbyssTags.Ability.Hero.Primary", "Tag for the Primary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "AbyssTags.Ability.Hero.Secondary", "Tag for the Secondary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "AbyssTags.Ability.Hero.Tertiary", "Tag for the Tertiary Ability")
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "AbyssTags.Ability.Enemy.HitReact","")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "AbyssTags.Ability.Enemy.Attack", "Tag for the Attack Ability")
		}
	}
	
	namespace Cooldown
	{
		
		namespace Melee
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SwordLightAttack, "AbyssTags.Cooldown.Melee.SwordLightAttack", "Tag for Ability that Melee LightAttack.")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SwordHeavyAttack, "AbyssTags.Cooldown.Melee.SwordHeavyAttack", "Tag for Ability that Melee HeavyAttack");
		}

		namespace Projectile
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireBalt, "AbyssTags.Cooldown.Projectile.FireBalt", "Tag for Ability that FireBalt Projectile.")
		}
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "AbyssTags.Cooldown.ActivateOnGiven", "Tag for Ability that should activate immediately once given.")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "AbyssTags.Cooldown.Death", "Tag for Ability that should be Add when the owner dies.")
		namespace Hero
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "AbyssTags.Cooldown.Hero.Primary", "Tag for the Primary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "AbyssTags.Cooldown.Hero.Secondary", "Tag for the Secondary Ability")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "AbyssTags.Cooldown.Hero.Tertiary", "Tag for the Tertiary Ability")
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "AbyssTags.Cooldown.Enemy.HitReact","")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "AbyssTags.Cooldown.Enemy.Attack", "Tag for the Attack Ability")
		}
	}
	
	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "AbyssTags.Events.KillScored", "Tag for the KillScored Event");
		
		namespace Montage
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "AbyssTags.Events.Montage.FireBolt", "Tag for the Montage notify cast FireBolt")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ElectrocuteBeam, "AbyssTags.Events.Montage.ElectrocuteBeam", "Tag for the Montage notify cast ElectrocuteBeam")
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "AbyssTags.Events.Enemy.MeleeTraceHit", "Tag for the Enemy MeleeTraceHit")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "AbyssTags.Events.Enemy.HitReact", "Tag for the Enemy HitReact")
		}
		
	}
	
	namespace Block
	{
		namespace Input
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityPressed, "AbyssTags.Block.Input.AbilityPressed", "Tag for Block Input AbilityPressed Callback")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityHeld, "AbyssTags.Block.Input.AbilityHeld", "Tag for Block AbilityInput Held Callback")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityReleased, "AbyssTags.Block.Input.AbilityReleased", "Tag for Block Input AbilityReleased Callback")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Move, "AbyssTags.Block.Input.Move", "Tag for Block Input Move Callback")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Jump, "AbyssTags.Block.Input.Jump", "Tag for Block Input Jump Callback")
		}
	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "AbyssTags.Status.Dead", "Tag for the Dead Status");
	}

	namespace Attributes
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "AbyssTags.Attributes.Health", "Tag for the Health Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxHealth, "AbyssTags.Attributes.MaxHealth", "Tag for the MaxHealth Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "AbyssTags.Attributes.Mana", "Tag for the Mana Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxMana,	"AbyssTags.Attributes.MaxMana", "Tag for the MaxMana Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stamina, "AbyssTags.Attributes.Stamina", "Tag for the Stamina Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxStamina, "AbyssTags.Attributes.MaxStamina", "Tag for the Max Stamina Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalPower, "AbyssTags.Attributes.PhysicalPower", "Tag for the PhysicalPower Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagicPower, "AbyssTags.Attributes.MagicPower", "Tag for the MagicPower Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Defense, "AbyssTags.Attributes.Defense", "Tag for the Defense Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CritRate, "AbyssTags.Attributes.CritRate", "Tag for the CritRate Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CritDamage, "AbyssTags.Attributes.CritDamage", "Tag for the CritDamage Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockChance, "AbyssTags.Attributes.BlockChance", "Tag for the BlockChance Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagicPenetration, "AbyssTags.Attributes.MagicPenetration", "Tag for the MagicPenetration Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalPenetration, "AbyssTags.Attributes.PhysicalPenetration", "Tag for the PhysicalPenetration Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(AttackSpeed, "AbyssTags.Attributes.AttackSpeed", "Tag for the AttackSpeed Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MoveSpeed, "AbyssTags.Attributes.MoveSpeed", "Tag for the MoveSpeed Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CooldownReduction, "AbyssTags.Attributes.CooldownReduction", "Tag for the CooldownReduction Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalResistance, "AbyssTags.Attributes.PhysicalResistance", "Tag for the PhysicalResistance Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagicResistance, "AbyssTags.Attributes.MagicResistance", "Tag for the MagicResistance Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SkillPoints, "AbyssTags.Attributes.SkillPoints", "Tag for the SkillPoints Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TalentPoints, "AbyssTags.Attributes.TalentPoints", "Tag for the TalentPoints Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SkillSlotAmount, "AbyssTags.Attributes.SkillSlotAmount", "Tag for the SkillSlotAmount Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CurrentXP, "AbyssTags.Attributes.CurrentXP", "Tag for the CurrentXP Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Level, "AbyssTags.Attributes.Level", "Tag for the Level Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(XPMultiplier, "AbyssTags.Attributes.XPMultiplier", "Tag for the XPMultiplier Attribute");
		
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncomingDamage, "AbyssTags.Attributes.IncomingDamage", "Tag for the IncomingDamage Attribute");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncomingXP, "AbyssTags.Attributes.IncomingXP", "Tag for the IncomingXP Attribute");
		
	}

	namespace InputTag
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LMB, "AbyssTags.InputTag.LMB", "Tag for the LMB key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RMB, "AbyssTags.InputTag.RMB", "Tag for the RMB key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(L_Shift, "AbyssTags.InputTag.L_Shift", "Tag for the L_Shift key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(L_Ctrl, "AbyssTags.InputTag.L_Ctrl", "Tag for the L_Ctrl key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(L_Alt, "AbyssTags.InputTag.L_Alt", "Tag for the L_Alt key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Super, "AbyssTags.InputTag.Super", "Tag for the Super key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(A, "AbyssTags.InputTag.A", "Tag for the A key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(B, "AbyssTags.InputTag.B", "Tag for the B key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(C, "AbyssTags.InputTag.C", "Tag for the C key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(D, "AbyssTags.InputTag.D", "Tag for the D key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(E, "AbyssTags.InputTag.E", "Tag for the E key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(F, "AbyssTags.InputTag.F", "Tag for the F key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(G, "AbyssTags.InputTag.G", "Tag for the G key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(H, "AbyssTags.InputTag.H", "Tag for the H key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(I, "AbyssTags.InputTag.I", "Tag for the I key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(J, "AbyssTags.InputTag.J", "Tag for the J key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(K, "AbyssTags.InputTag.K", "Tag for the K key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(L, "AbyssTags.InputTag.L", "Tag for the L key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(M, "AbyssTags.InputTag.M", "Tag for the M key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(N, "AbyssTags.InputTag.N", "Tag for the N key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(O, "AbyssTags.InputTag.O", "Tag for the O key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(P, "AbyssTags.InputTag.P", "Tag for the P key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Q, "AbyssTags.InputTag.Q", "Tag for the Q key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(R, "AbyssTags.InputTag.R", "Tag for the R key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(S, "AbyssTags.InputTag.S", "Tag for the S key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(T, "AbyssTags.InputTag.T", "Tag for the T key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(U, "AbyssTags.InputTag.U", "Tag for the U key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(V, "AbyssTags.InputTag.V", "Tag for the V key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(W, "AbyssTags.InputTag.W", "Tag for the W key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(X, "AbyssTags.InputTag.X", "Tag for the X key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Y, "AbyssTags.InputTag.Y", "Tag for the Y key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Z, "AbyssTags.InputTag.Z", "Tag for the Z key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num1, "AbyssTags.InputTag.num1", "Tag for the num1 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num2, "AbyssTags.InputTag.num2", "Tag for the num2 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num3, "AbyssTags.InputTag.num3", "Tag for the num3 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num4, "AbyssTags.InputTag.num4", "Tag for the num4 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num5, "AbyssTags.InputTag.num5", "Tag for the num5 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num6, "AbyssTags.InputTag.num6", "Tag for the num6 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num7, "AbyssTags.InputTag.num7", "Tag for the num7 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num8, "AbyssTags.InputTag.num8", "Tag for the num8 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num9, "AbyssTags.InputTag.num9", "Tag for the num9 key");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(num0, "AbyssTags.InputTag.num0", "Tag for the num0 key");
	}
	
	
	/**
	 * 
	 */
	namespace GlobalData
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GlobalData, "AbyssTags.GlobalData", "Tag for GlobalData");
	}
}
