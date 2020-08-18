// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"
#include "Blueprint/WidgetTree.h"



bool UStoreWidget::Initialize()
{
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
    for (int i = 0; i < item_count; ++i)
    {
        auto l_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("Button%d",i));
        l_button->SetVisibility(ESlateVisibility::Visible);
        buttons.Add(l_button);
        //auto button_func = ::CreateStatic([&, l_button]() { this->OnItemClicked(l_button); });
        //click_delegate.AddLambda([&, l_button]() { this->OnItemClicked(l_button); });
        //click_delegate.CreateLambda([&, l_button]() { this->OnItemClicked(l_button); });
        TFunction<void(void)> test_func;
        l_button->load.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //on_item_click = [&, l_button]() { this->OnItemClicked(l_button); };
        //l_button->OnClicked.AddDynamic(this, &UStoreWidget::OnItemClicked);
        //l_button->OnClicked.AddDynamic(this, test_func);
        auto slot = grid->AddChildToUniformGrid(l_button, i / col_count, i % col_count);
        slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        row_count = i / col_count + 1;
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
    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)

}