// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"
#include "Blueprint/WidgetTree.h"
#include "AghsCloneCharacter.h"
#include "AghsClonePlayerController.h"
#include "UnitController.h"
#include "Shop.h"


bool UStoreWidget::Initialize()
{
    
    auto buyer = Cast<AUnitController>(GetWorld()->GetFirstPlayerController()->GetPawn())->GetPrimaryUnit();
    if (!buyer)
    {
        return false;
    }
    AShop* shop = AShop::GetClosestShop(buyer);
    int32 max_col_count = 5;
    int32 col_count = 0;
    int32 item_count = 22;
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
    auto shop_items = shop->GetItems();
    //for (int i = 0; i < item_count; ++i)
    int count = 0;
    for (auto& si : shop_items)
    {
        auto l_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("Button%d",count));
        
        l_button->SetVisibility(ESlateVisibility::Visible);
        buttons.Add(l_button, si.Key);
        //auto button_func = ::CreateStatic([&, l_button]() { this->OnItemClicked(l_button); });
        //click_delegate.AddLambda([&, l_button]() { this->OnItemClicked(l_button); });
        //click_delegate.CreateLambda([&, l_button]() { this->OnItemClicked(l_button); });
        TFunction<void(void)> test_func;
        l_button->RightClick.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //on_item_click = [&, l_button]() { this->OnItemClicked(l_button); };
        //l_button->OnClicked.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //l_button->OnClicked.AddDynamic(this, test_func);
        auto slot = grid->AddChildToUniformGrid(l_button, count / max_col_count, count % max_col_count);
        l_button->SetBrushFromTexture(si.Value.proto->GetMaterial());
        slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        count += 1;
        col_count = std::min(count, max_col_count);
        row_count = count / max_col_count + 1;
    }
    SetDesiredSizeInViewport(FVector2D(el_width*col_count, el_height*row_count));
    SetVisibility(ESlateVisibility::Visible);
    return true;
}

void UStoreWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UStoreWidget::OnItemClicked(UMultiButton* in_button)
{
    
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    pc->RequestBuy(buttons[in_button]);

    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)

}

void UStoreWidget::OnItemHovered(UMultiButton* in_button)
{
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc->WasInputKeyJustPressed(EKeys::RightMouseButton))
    {
        pc->RequestBuy(buttons[in_button]);

        UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
    }

}

bool UStoreWidget::GridIsHovered() const
{
    return grid->IsHovered();
}