#include "AbilityWidget.h"

bool UAbilityWidget::Initialize()
{
    Super::Initialize();
    grid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), "Grid");
    grid->SetVisibility(ESlateVisibility::Visible);
    SetVisibility(ESlateVisibility::Visible);
    WidgetTree->RootWidget = grid;
    grid->SetIsEnabled(true);

    overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("Overlay"));
    auto spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), FName("Spacer"));
    spacer->SetSize(FVector2D(160, 160));
    ability_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("AbilityButton"));
    auto slot = grid->AddChildToGrid(ability_button, 0, 0);
    overlay->AddChild(spacer);
    slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
    slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

    cooldown = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("Cooldown"));
    cooldown->SetText(FText::FromString(""));
    cooldown->ColorAndOpacity = FSlateColor(FLinearColor(0.4, 0.4, 0.4));
    cooldown->Font.Size = 30;
    ability_button->AddChild(overlay);
    overlay->AddChild(cooldown);

    level = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("Level"));
    level->SetText(FText::FromString(""));
    level->ColorAndOpacity = FSlateColor(FLinearColor(0.4, 0.4, 0.4));
    level->Font.Size = 30;
    grid->AddChildToGrid(level, 1, 0);

    el_height = 160;
    el_width = 160;
    return true;
}

bool UAbilityWidget::Refresh()
{
    ability_button->SetBrushFromTexture(ability->GetMaterial());
    if (ability)
    {
        float cd = ability->GetCurrentCooldown();
        if (cd)
        {
            cooldown->SetText(FText::FromString(FString::FromInt(int32(std::ceil(cd)))));
        }
        else
        {
            cooldown->SetText(FText::FromString(""));
        }
        int current_level = ability->GetLevel();
        {
            level->SetText(FText::FromString(FString::FromInt(current_level)));
        }
    }
    return true;
}

void UAbilityWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UAbilityWidget::OnItemClicked(UMultiButton* in_button)
{
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));

    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
}

bool UAbilitiesWidget::Initialize()
{
    Super::Initialize();
    //main_back = WidgetTree->ConstructWidget<UPane>(UMultiButton::StaticClass(), "base");
    grid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), "Grid");
    grid->SetVisibility(ESlateVisibility::Visible);
    SetVisibility(ESlateVisibility::Visible);
    WidgetTree->RootWidget = grid;
    grid->SetIsEnabled(true);
    el_height = 160;
    el_width = 160;
    grid->SetMinDesiredSlotHeight(el_height);
    grid->SetMinDesiredSlotWidth(el_width);
    grid->SetSlotPadding(FMargin(5, 5));
    return Refresh();
}

bool UAbilitiesWidget::Refresh()
{
    int32 child_count = grid->GetChildrenCount();
    if (child_count != abilities.Num())
    {
        grid->ClearChildren();
        buttons.Empty();
        for (int i = 0; i < abilities.Num(); ++i)
        {
            auto ab_widget = WidgetTree->ConstructWidget<UAbilityWidget>(UAbilityWidget::StaticClass(), FName("ab_widget", i));
            
            auto slot = grid->AddChildToUniformGrid(ab_widget, 0, i);
            slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
            slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
            ab_widget->SetDesiredSizeInViewport(FVector2D(160, 160));

        }
    }
    for (int i = 0; i < abilities.Num(); ++i)
    {
        auto child = Cast<UAbilityWidget>(grid->GetChildAt(i));
        child->ability = abilities[i];
        child->Refresh();
    }
    return true;

}

void UAbilitiesWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UAbilitiesWidget::SetAbilities()
{
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc)
    {
        auto uc = Cast<AUnitController>(pc->GetPawn());
        if (uc)
        {
            auto prime_unit = uc->GetPrimaryUnit();
            auto new_abilities = prime_unit->GetAbilityArray();
            abilities = new_abilities;
            int count = 0;
        }
    }
}