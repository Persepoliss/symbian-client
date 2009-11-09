/*
============================================================================
 Name        : BuddycloudBeaconSettingsList.h
 Author      : Ross Savage
 Copyright   : Buddycloud 2008
 Description : Declares Settings List
============================================================================
*/

#include <aknnavide.h>
#include <akntabgrp.h>
#include <barsread.h>
#include <eikspane.h>
#include <Buddycloud.rsg>
#include "Buddycloud.hrh"
#include "Buddycloud.hlp.hrh"
#include "BuddycloudBeaconSettingsList.h"
#include "BuddycloudConstants.h"

CBuddycloudBeaconSettingsList::CBuddycloudBeaconSettingsList(CBuddycloudLogic* aBuddycloudLogic) : CAknSettingItemList() {
	iBuddycloudLogic = aBuddycloudLogic;
}

void CBuddycloudBeaconSettingsList::ConstructL(const TRect& aRect) {
	iNaviPane = (CAknNavigationControlContainer*) iEikonEnv->AppUiFactory()->StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	TResourceReader aNaviReader;
	iEikonEnv->CreateResourceReaderLC(aNaviReader, R_SETTINGS_NAVI_DECORATOR);
	iNaviDecorator = iNaviPane->ConstructNavigationDecoratorFromResourceL(aNaviReader);
	CleanupStack::PopAndDestroy();

	CAknTabGroup* aTabGroup = (CAknTabGroup*)iNaviDecorator->DecoratedControl();
	aTabGroup->SetActiveTabById(ETabBeacons);

	iNaviPane->PushL(*iNaviDecorator);

	SetRect(aRect);
}

CBuddycloudBeaconSettingsList::~CBuddycloudBeaconSettingsList() {
	iNaviPane->Pop(iNaviDecorator);

	if(iNaviDecorator)
		delete iNaviDecorator;
}

void CBuddycloudBeaconSettingsList::EditCurrentItemL() {
	EditItemL(ListBox()->CurrentItemIndex(), false);
}

void CBuddycloudBeaconSettingsList::GetHelpContext(TCoeHelpContext& aContext) const {
	aContext.iMajor = TUid::Uid(HLPUID);
	aContext.iContext = KPositioningSettings;
}

TKeyResponse CBuddycloudBeaconSettingsList::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) {
	TKeyResponse aResult = EKeyWasNotConsumed;

	if(aType == EEventKey) {
		if(aKeyEvent.iCode == EKeyLeftArrow) {
			iCoeEnv->AppUi()->ActivateViewL(TVwsViewId(TUid::Uid(APPUID), KNotificationsSettingsViewId));

			aResult = EKeyWasConsumed;
		}
		else if(aKeyEvent.iCode == EKeyRightArrow) {
			iCoeEnv->AppUi()->ActivateViewL(TVwsViewId(TUid::Uid(APPUID), KAccountSettingsViewId));

			aResult = EKeyWasConsumed;
		}
	}
	
	if(aResult == EKeyWasNotConsumed) {
		CAknSettingItemList::OfferKeyEventL(aKeyEvent, aType);
	}
	
	return aResult;
}

void CBuddycloudBeaconSettingsList::EditItemL(TInt aIndex, TBool aCalledFromMenu) {
	CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
	
	CAknSettingItemArray* aItemArray = SettingItemArray();
	((*aItemArray)[aIndex])->StoreL();
	((*aItemArray)[aIndex])->UpdateListBoxTextL();
	
	switch(((*aItemArray)[aIndex])->Identifier()) {
		case ESettingPositioningCellOn:
			iBuddycloudLogic->SetLocationResource(EResourceCell, iBuddycloudLogic->GetBoolSetting(ESettingItemCellOn));
			break;
		case ESettingPositioningWifiOn:
			iBuddycloudLogic->SetLocationResource(EResourceWlan, iBuddycloudLogic->GetBoolSetting(ESettingItemWifiOn));
			break;
		case ESettingPositioningBtOn:
			iBuddycloudLogic->SetLocationResource(EResourceBt, iBuddycloudLogic->GetBoolSetting(ESettingItemBtOn));
			break;
		case ESettingPositioningGpsOn:
			iBuddycloudLogic->SetLocationResource(EResourceGps, iBuddycloudLogic->GetBoolSetting(ESettingItemGpsOn));
			break;
		default:;
	}
}

CAknSettingItem* CBuddycloudBeaconSettingsList::CreateSettingItemL (TInt aIdentifier) {
	CAknSettingItem* aSettingItem = NULL;

	switch(aIdentifier) {
		case ESettingPositioningCellOn:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iBuddycloudLogic->GetBoolSetting(ESettingItemCellOn));
			break;
		case ESettingPositioningWifiOn:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iBuddycloudLogic->GetBoolSetting(ESettingItemWifiOn));
			break;
		case ESettingPositioningBtOn:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iBuddycloudLogic->GetBoolSetting(ESettingItemBtOn));
			break;
		case ESettingPositioningGpsOn:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iBuddycloudLogic->GetBoolSetting(ESettingItemGpsOn));
			break;
		default:
			break;
    }

    return aSettingItem;
}
