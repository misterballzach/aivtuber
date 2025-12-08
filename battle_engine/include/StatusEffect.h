#pragma once

#include "CoreTypes.h"

class StatusEffect {
public:
    FString Name;
    FString Description;
    int32 Value;      // Stack amount or intensity
    int32 Duration;   // Turns remaining
    bool bIsDebuff;   // True for debuffs, false for buffs

    StatusEffect(FString InName, FString InDesc, int32 InValue, int32 InDuration, bool bInIsDebuff)
        : Name(InName), Description(InDesc), Value(InValue), Duration(InDuration), bIsDebuff(bInIsDebuff) {}

    virtual ~StatusEffect() = default;

    // Hooks
    virtual void OnApply(Combatant* Owner) {
        // Log or trigger visual
    }

    virtual void OnTurnStart(Combatant* Owner) {
        // e.g. Poison damage
    }

    virtual void OnTurnEnd(Combatant* Owner) {
        // Default behavior: decrement duration
        if (Duration > 0) {
            Duration--;
        }
    }

    virtual float32 GetDamageModifier(float32 IncomingDamage, Combatant* Owner) {
        return IncomingDamage; // Default no change
    }

    virtual float32 GetOutputDamageModifier(float32 OutgoingDamage, Combatant* Owner) {
        return OutgoingDamage; // Default no change
    }

    // Factory method for cloning (prototype pattern)
    virtual TSharedPtr<StatusEffect> Clone() const = 0;
};

// --- Concrete Examples ---

class StatusWeak : public StatusEffect {
public:
    StatusWeak(int32 InDuration)
        : StatusEffect("Weak", "Deal 25% less damage.", 0, InDuration, true) {}

    float32 GetOutputDamageModifier(float32 OutgoingDamage, Combatant* Owner) override {
        return OutgoingDamage * 0.75f;
    }

    TSharedPtr<StatusEffect> Clone() const override {
        return std::make_shared<StatusWeak>(Duration);
    }
};

class StatusVulnerable : public StatusEffect {
public:
    StatusVulnerable(int32 InDuration)
        : StatusEffect("Vulnerable", "Take 50% more damage.", 0, InDuration, true) {}

    float32 GetDamageModifier(float32 IncomingDamage, Combatant* Owner) override {
        return IncomingDamage * 1.5f;
    }

    TSharedPtr<StatusEffect> Clone() const override {
        return std::make_shared<StatusVulnerable>(Duration);
    }
};

class StatusStrength : public StatusEffect {
public:
    StatusStrength(int32 InValue)
        : StatusEffect("Strength", "Deal X more damage.", InValue, -1, false) {} // -1 duration = permanent until removed

    float32 GetOutputDamageModifier(float32 OutgoingDamage, Combatant* Owner) override {
        return OutgoingDamage + Value;
    }

    // Strength usually doesn't decay by duration in some games, but let's assume it's permanent stack-based
    void OnTurnEnd(Combatant* Owner) override {
        // Do not decrement duration
    }

    TSharedPtr<StatusEffect> Clone() const override {
        return std::make_shared<StatusStrength>(Value);
    }
};
