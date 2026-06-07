#include "ProjectKR_AbilitySystemComponent.h"

// Sets default values for this component's properties
UProjectKR_AbilitySystemComponent::UProjectKR_AbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UProjectKR_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UProjectKR_AbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProjectKR_AbilitySystemComponent::InputTagPressed(const struct FGameplayTag& InInputTag)
{
	if(InInputTag.IsValid() == false)
		return;

	ABILITYLIST_SCOPE_LOCK();
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.Ability == nullptr)
			continue;

		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) == false)
			continue;

		if(AbilitySpec.IsActive() == true)
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		else
			TryActivateAbility(AbilitySpec.Handle);
	}
}
void UProjectKR_AbilitySystemComponent::InputTagReleased(const struct FGameplayTag& InInputTag)
{
	if(InInputTag.IsValid() == false)
		return;

	ABILITYLIST_SCOPE_LOCK();
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.Ability == nullptr)
			continue;

		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) == false)
			continue;

		if(AbilitySpec.IsActive() == true)
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
	}
}

FGameplayAbilitySpecHandle UProjectKR_AbilitySystemComponent::GiveAbilityWithInputTag(TSubclassOf<class UGameplayAbility> InAbilityClass, const struct FGameplayTag& InInputTag, int32 InLevel)
{
	if(InAbilityClass == nullptr)
		return FGameplayAbilitySpecHandle();

	FGameplayAbilitySpec AbilitySpec(InAbilityClass, InLevel, INDEX_NONE, this);
	if(InInputTag.IsValid() == false)
		AbilitySpec.DynamicAbilityTags.AddTag(InInputTag);

	return GiveAbility(AbilitySpec);
}

