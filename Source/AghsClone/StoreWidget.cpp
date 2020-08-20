// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
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
        row_count = std::ceil(count * 1.0 / max_col_count);
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
    if (IsValid(grid))
    {
        return grid->IsHovered();
    }
    return false;
}

bool UInventoryWidget::Initialize()
{
    Super::Initialize();

    grid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), "Grid");
    grid->SetVisibility(ESlateVisibility::Visible);
    SetVisibility(ESlateVisibility::Visible);
    WidgetTree->RootWidget = grid;
    grid->SetIsEnabled(true);
    el_height = 80;
    el_width = 80;
    grid->SetMinDesiredSlotHeight(el_height);
    grid->SetMinDesiredSlotWidth(el_width);
    grid->SetSlotPadding(FMargin(5, 5));
    SetItems();
    return DrawInventory();
}

bool UInventoryWidget::DrawInventory()
{
    if (!grid)
    {
        return false;
    }
    int32 max_col_count = 3;
    int32 col_count = 0;
    grid->ClearChildren();
    int row_count = 1;
    TArray<AItem*> shop_items;
    if (current_inventory)
    {
        shop_items = current_inventory->GetItems();
    }
    else
    {
        for (int i = 0; i < 6; ++i)
        {
            shop_items.Add(nullptr);
        }
    }
    int count = 0;
    for (auto& si : shop_items)
    {
        auto l_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("Button%d",count));
        
        l_button->SetVisibility(ESlateVisibility::Visible);
        buttons.Add(l_button, si);
        TFunction<void(void)> test_func;
        l_button->RightClick.AddDynamic(this, &UInventoryWidget::OnItemClicked);
        auto slot = grid->AddChildToUniformGrid(l_button, count / max_col_count, count % max_col_count);
        if (si)
        {
            l_button->SetBrushFromTexture(si->GetMaterial());
        }
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

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UInventoryWidget::OnItemClicked(UMultiButton* in_button)
{
    
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));

    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
}

void UInventoryWidget::OnItemHovered(UMultiButton* in_button)
{
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc->WasInputKeyJustPressed(EKeys::RightMouseButton))
    {
        UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
    }

}

void UInventoryWidget::SetItems()
{
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc)
    {
        auto uc = Cast<AUnitController>(pc->GetPawn());
        if (uc)
        {
            auto prime_unit = uc->GetPrimaryUnit();
            auto inventory = Cast<UInventoryComponent>(prime_unit->GetComponentByClass(UInventoryComponent::StaticClass()));
            if (inventory)
            {
                current_inventory = inventory; 
            }
        }
    }
}

bool UInventoryWidget::GridIsHovered() const
{
    return grid->IsHovered();
}

bool UAbilitiesWidget::Initialize()
{
    Super::Initialize();

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
    

    SetAbilities();
    return DrawAbilities();
}

bool UAbilitiesWidget::DrawAbilities()
{
    int32 max_col_count = 6;
    int32 col_count = 0;
    grid->ClearChildren();
    int row_count = 1;
    
    int count = 0;
    for (auto& ab : abilities)
    {
        auto l_button = WidgetTree->ConstructWidget<UMultiButton>(UMultiButton::StaticClass(), FName("Button",count));
        
        l_button->SetVisibility(ESlateVisibility::Visible);
        buttons.Add(l_button, ab);
        TFunction<void(void)> test_func;
        l_button->RightClick.AddDynamic(this, &UAbilitiesWidget::OnItemClicked);
        auto slot = grid->AddChildToUniformGrid(l_button, count / max_col_count, count % max_col_count);
        if (ab)
        {
            l_button->SetBrushFromTexture(ab->GetMaterial());
        }
        slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        count += 1;
        col_count = std::min(count, max_col_count);
        row_count = std::ceil(count*1.0 / max_col_count);
        auto cooldown = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("Cooldown", count));
      
        cooldown->SetText(FText::FromString(""));
        cooldown->ColorAndOpacity = FSlateColor(FLinearColor(0.4, 0.4, 0.4));
        cooldown->Font.Size = 30;
        l_button->AddChild(cooldown);
    }
    SetDesiredSizeInViewport(FVector2D(el_width * col_count, el_height*row_count));
    SetVisibility(ESlateVisibility::Visible);
    return true;
}

void UAbilitiesWidget::RefreshCooldownDisplays()
{
    for (auto& b : buttons)
    {
        if (b.Key->GetChildrenCount())
        {
            auto tb = Cast<UTextBlock>(b.Key->GetChildAt(0));
            if (tb)
            {
                float cd = b.Value->GetCurrentCooldown();
                if (cd)
                {
                    tb->SetText(FText::FromString(FString::FromInt(int32(std::ceil(cd)))));
                }
                else
                {
                    tb->SetText(FText::FromString(""));
                }
            }
        }
    }
}

void UAbilitiesWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.


}

void UAbilitiesWidget::OnItemClicked(UMultiButton* in_button)
{
    
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));

    UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
}

void UAbilitiesWidget::OnItemHovered(UMultiButton* in_button)
{
    auto child_index = grid->GetChildIndex(in_button);
    auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld()));
    if (pc->WasInputKeyJustPressed(EKeys::RightMouseButton))
    {
        UE_LOG(LogTemp, Warning, TEXT("Bought %d"), child_index)
    }

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
        }
    }
}

bool UAbilitiesWidget::GridIsHovered() const
{
    return grid->IsHovered();
}
