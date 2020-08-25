

#include "StoreWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/Spacer.h"
#include "AghsClonePlayerController.h"
#include "AbilityWidget.generated.h"

UCLASS()
class AGHSCLONE_API UAbilityWidget: public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UGridPanel* grid;
    UAbility* ability;
    UMultiButton* ability_button;
	UOverlay* overlay;
    UTextBlock* level;
	UTextBlock* cooldown;
	virtual bool Initialize() override;

	bool Refresh();

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);
};

UCLASS()
class AGHSCLONE_API UAbilitiesWidget: public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UUniformGridPanel* grid;
	TMap<UAbilityWidget*, UAbility*> buttons;
    TArray<UAbility*> abilities;
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void SetAbilities();
	bool Refresh();
};
