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
    int32 col_count = 5;
    int32 item_count = 22;
    Super::Initialize();
    //button = CreateDefaultSubobject<UButton>("Button");
    grid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), "Grid");
    grid->SetVisibility(ESlateVisibility::Visible);
    SetVisibility(ESlateVisibility::Visible);
    WidgetTree->RootWidget = grid;
    grid->SetIsEnabled(true);
    grid->SetMinDesiredSlotHeight(20);
    grid->SetMinDesiredSlotWidth(20);
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
        l_button->load.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //on_item_click = [&, l_button]() { this->OnItemClicked(l_button); };
        //l_button->OnClicked.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //l_button->OnClicked.AddDynamic(this, test_func);
        auto slot = grid->AddChildToUniformGrid(l_button, count / col_count, count % col_count);
        slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        row_count = count / col_count + 1;
        count += 1;
    }
    SetDesiredSizeInViewport(FVector2D(400, row_count * 400.0 / col_count));
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