// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "AghsCloneCharacter.h"
#include "AghsClonePlayerController.h"
#include "AghsCloneGameMode.h"
#include "UnitController.h"
#include "CharacterSelectWidget.h"
#include "Shop.h"


bool UCharacterSelectWidget::Initialize()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameModeInfo::StaticClass(), FoundActors);
    if (FoundActors.Num())
    {
        character_list = Cast<AGameModeInfo>(FoundActors[0])->GetCharacterList();
    }
    int32 max_col_count = 5;
    int32 col_count = 0;
    Super::Initialize();
    //button = CreateDefaultSubobject<UButton>("Button");
    grid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), "Grid");
    grid->SetVisibility(ESlateVisibility::Visible);
    SetVisibility(ESlateVisibility::Visible);
    WidgetTree->RootWidget = grid;
    grid->SetIsEnabled(true);
    int32 el_height = 80;
    int32 el_width = 80;
    grid->SetMinDesiredSlotHeight(el_height);
    grid->SetMinDesiredSlotWidth(el_width);
    grid->SetSlotPadding(FMargin(5, 5));
    //button->SetRenderScale(FVector2D(10, 10));
    int row_count = 1;
    int count = 0;
    for (auto& chr : character_list)
    {
        auto l_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("Button%d",count));
        
        l_button->SetVisibility(ESlateVisibility::Visible);
        buttons.Add(l_button, count);
        l_button->LeftClick.AddDynamic(this, &UCharacterSelectWidget::OnItemClicked);
        auto slot = grid->AddChildToUniformGrid(l_button, count / max_col_count, count % max_col_count);
        l_button->SetBrushFromTexture(chr.MyTex);
        slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        count += 1;
        col_count = std::min(count, max_col_count);
        row_count = std::ceil(count * 1.0 / max_col_count);
    }
    SetDesiredSizeInViewport(FVector2D(el_width*col_count, el_height*row_count));
    SetVisibility(ESlateVisibility::Visible);
    return true;
}

void UCharacterSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UCharacterSelectWidget::OnItemClicked(UMultiButton* in_button)
{
    
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    pc->RequestBuy(buttons[in_button]);

    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)

}

void UCharacterSelectWidget::OnItemHovered(UMultiButton* in_button)
{
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc->WasInputKeyJustPressed(EKeys::RightMouseButton))
    {
        pc->RequestBuy(buttons[in_button]);

        UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
    }

}

bool UCharacterSelectWidget::GridIsHovered() const
{
    if (IsValid(grid))
    {
        return grid->IsHovered();
    }
    return false;
}