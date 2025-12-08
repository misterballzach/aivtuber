#include "../include/Card.h"
#include "../include/Combatant.h"
#include "../include/StatusEffect.h"

// Implementation of Card logic to avoid circular dependencies in headers

void AttackCard::Execute(const FCombatContext& Context) {
    if (Context.Target) {
        float32 FinalDamage = Context.Source->GetOutgoingDamageModifier((float32)DamageAmount);
        Context.Target->TakeDamage((int32)FinalDamage, Context.Source);
    }
}

void BlockCard::Execute(const FCombatContext& Context) {
    if (Context.Source) {
        Context.Source->GainBlock(BlockAmount);
    }
}

void EffectCard::Execute(const FCombatContext& Context) {
    if (Context.Target) {
        Context.Target->ApplyStatus(EffectToApply->Clone());
    }
}

void Enemy::ExecuteTurn(Combatant* Target, BattleManager* Manager) {
    std::cout << Name << " intends to " << (NextMove == EIntent::Attack ? "Attack" : (NextMove == EIntent::Defend ? "Defend" : "Debuff")) << std::endl;

    switch (NextMove) {
        case EIntent::Attack:
            if (Target) {
                float32 Dmg = GetOutgoingDamageModifier((float32)IntentValue);
                Target->TakeDamage((int32)Dmg, this);
            }
            break;
        case EIntent::Defend:
            GainBlock(IntentValue);
            break;
        case EIntent::Debuff:
            if (Target) {
                Target->ApplyStatus(std::make_shared<StatusWeak>(IntentValue));
            }
            break;
    }
}
