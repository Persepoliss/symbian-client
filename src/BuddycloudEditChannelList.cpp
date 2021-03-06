/*
============================================================================
 Name        : BuddycloudEditChannelList.h
 Author      : Ross Savage
 Copyright   : 2008 Buddycloud
 Description : Declares Edit Channel List
============================================================================
*/

#include <aknnotewrappers.h>
#include <akntitle.h>
#include <akntextsettingpage.h>
#include <eikspane.h>
#include <Buddycloud_lang.rsg>
#include "Buddycloud.hrh"
#include "BuddycloudConstants.h"
#include "BuddycloudEditChannelList.h"
#include "XmlParser.h"
#include "XmppUtilities.h"

CBuddycloudEditChannelList::CBuddycloudEditChannelList(CBuddycloudLogic* aBuddycloudLogic) : CAknSettingItemList() {
	iBuddycloudLogic = aBuddycloudLogic;
	iXmppInterface = iBuddycloudLogic->GetXmppInterface();
	
	iTitleResourceId = R_LOCALIZED_STRING_NEWCHANNEL_TITLE;
}

void CBuddycloudEditChannelList::ConstructL(const TRect& aRect, TViewData aQueryData) {
	iQueryData = aQueryData;
	
	iTextUtilities = CTextUtilities::NewL();
	
	iChannelItem = static_cast <CFollowingChannelItem*> (iBuddycloudLogic->GetFollowingStore()->GetItemById(iQueryData.iId));

	if(iChannelItem == NULL) {		
		TPtrC aEncData(iTextUtilities->Utf8ToUnicodeL(iQueryData.iData));
		
		iId.Copy(aEncData.Left(iId.MaxLength()));
		
		CXmppChannelIdUtilities::ValidateId(iId);
		
		if(iId.Length() > 0) {
			iChannelSaveAllowed = true;
		}
			
		iChannelEditAllowed = true;
	}
	
	LoadChannelDataL();
	SetTitleL(iTitleResourceId);
	SetRect(aRect);
}

CBuddycloudEditChannelList::~CBuddycloudEditChannelList() {
	iXmppInterface->CancelXmppStanzaAcknowledge(this);
	
	if(iTextUtilities) {
		delete iTextUtilities;
	}
	
	if(iChannelItem && iChannelItem->GetItemId() <= 0) {	
		delete iChannelItem;
	}
}

void CBuddycloudEditChannelList::SetTitleL(TInt aResourceId) {
	CAknTitlePane* aTitlePane = static_cast<CAknTitlePane*>(iEikonEnv->AppUiFactory()->StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
	HBufC* aTitle = iEikonEnv->AllocReadResourceLC(aResourceId);
	aTitlePane->SetTextL(*aTitle);
	CleanupStack::PopAndDestroy(); // aTitle
}

void CBuddycloudEditChannelList::CollectChannelMetadataL(const TDesC& aNodeId) {
	TPtrC8 aEncNodeId(iTextUtilities->UnicodeToUtf8L(aNodeId));
	
	_LIT8(KDiscoItemsStanza, "<iq to='' type='get' id='metadata'><query xmlns='http://jabber.org/protocol/disco#info' node=''/></iq>\r\n");
	HBufC8* aDiscoItemsStanza = HBufC8::NewLC(KDiscoItemsStanza().Length() + KBuddycloudPubsubServer().Length() + aEncNodeId.Length());
	TPtr8 pDiscoItemsStanza(aDiscoItemsStanza->Des());
	pDiscoItemsStanza.Append(KDiscoItemsStanza);
	pDiscoItemsStanza.Insert(94, aEncNodeId);
	pDiscoItemsStanza.Insert(8, KBuddycloudPubsubServer);
	
	iXmppInterface->SendAndAcknowledgeXmppStanza(pDiscoItemsStanza, this, true, EXmppPriorityHigh);
	CleanupStack::PopAndDestroy(); // aDiscoItemsStanza
}

void CBuddycloudEditChannelList::EditCurrentItemL() {
	HandleListBoxEventL(ListBox(), EEventItemDoubleClicked);
}

void CBuddycloudEditChannelList::LoadChannelDataL() {
	if(iChannelItem) {
		iTitleResourceId = R_LOCALIZED_STRING_EDITCHANNEL_TITLE;	
		
		CollectChannelMetadataL(iChannelItem->GetId());
	}
}

TInt CBuddycloudEditChannelList::SaveChannelDataL() {
	StoreSettingsL();
	
	if(iChannelSaveAllowed) {
		// Convert affiliation
		iAffiliation = (iAffiliation == 0 ? EPubsubAffiliationMember : EPubsubAffiliationPublisher);
		
		iTextUtilities->FindAndReplace(iDescription, TChar(8233), TChar(10));
		
		if(iChannelItem && iChannelItem->GetItemType() == EItemChannel) {
			iChannelItem->SetTitleL(iTitle);
			iChannelItem->SetDescriptionL(iDescription);
		}
		
		if(iChannelItem) {
			// Store edited data
			if(iChannelItem->GetItemType() == EItemChannel) {
				iChannelItem->SetTitleL(iTitle);
				iChannelItem->SetDescriptionL(iDescription);
			}
			
			// Republish channel metadata
			HBufC8* aEncTitle = iTextUtilities->UnicodeToUtf8L(iTitle).AllocLC();
			HBufC8* aEncDescription = iTextUtilities->UnicodeToUtf8L(iDescription).AllocLC();
			TPtrC8 aAccess = CXmppEnumerationConverter::PubsubAccessModel((TXmppPubsubAccessModel)iAccess);
			TPtrC8 aAffiliation = CXmppEnumerationConverter::PubsubAffiliation((TXmppPubsubAffiliation)iAffiliation);
			TPtrC8 aEncId(iTextUtilities->UnicodeToUtf8L(iChannelItem->GetId()));
		
			// Send stanza
			_LIT8(KEditStanza, "<iq to='' type='set' id='editchannel'><pubsub xmlns='http://jabber.org/protocol/pubsub#owner'><configure node=''><x xmlns='jabber:x:data' type='submit'><field var='FORM_TYPE' type='hidden'><value>http://jabber.org/protocol/pubsub#node_config</value></field><field var='pubsub#access_model'><value></value></field><field var='pubsub#default_affiliation'><value></value></field><field var='pubsub#title'><value></value></field><field var='pubsub#description'><value></value></field></x></configure></pubsub></iq>\r\n");
			HBufC8* aEditStanza = HBufC8::NewLC(KEditStanza().Length() + KBuddycloudPubsubServer().Length() + aEncId.Length() + aAccess.Length() + aAffiliation.Length() + aEncTitle->Des().Length() + aEncDescription->Des().Length());
			TPtr8 pEditStanza(aEditStanza->Des());
			pEditStanza.Append(KEditStanza);
			pEditStanza.Insert(464, *aEncDescription);
			pEditStanza.Insert(409, *aEncTitle);
			pEditStanza.Insert(360, aAffiliation);
			pEditStanza.Insert(297, aAccess);
			pEditStanza.Insert(111, aEncId);
			pEditStanza.Insert(8, KBuddycloudPubsubServer);
		
			iXmppInterface->SendAndForgetXmppStanza(pEditStanza, true);
			CleanupStack::PopAndDestroy(3); // aEditStanza, aEncDescription, aEncTitle
		}
		else {
			// Create new channel
			return iBuddycloudLogic->CreateChannelL(iId, iTitle, iDescription, TXmppPubsubAccessModel(iAccess));
		}
	}
	
	return 0;
}

void CBuddycloudEditChannelList::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane) {
	if(aResourceId == R_SETTINGS_OPTIONS_MENU) {
		aMenuPane->SetItemDimmed(EMenuDeleteCommand, true);
		aMenuPane->SetItemDimmed(EMenuEditItemCommand, true);
		aMenuPane->SetItemDimmed(EAknSoftkeyDone, true);
		
		if(iChannelEditAllowed) {
			aMenuPane->SetItemDimmed(EMenuEditItemCommand, false);
		}
		
		if(iChannelSaveAllowed && iTitle.Length() > 0) {
			aMenuPane->SetItemDimmed(EAknSoftkeyDone, false);
		}
	}
}

void CBuddycloudEditChannelList::EditItemL(TInt aIndex, TBool aCalledFromMenu) {
	if(iChannelEditAllowed) {
		CAknSettingItemArray* aItemArray = SettingItemArray();
		TInt aIdentifier = ((*aItemArray)[aIndex])->Identifier();
	
		// Set title
		switch(aIdentifier) {
			case EEditChannelItemTitle:
				SetTitleL(R_LOCALIZED_STRING_EDITCHANNEL_TITLE_TITLE);
				break;
			case EEditChannelItemId:
				SetTitleL(R_LOCALIZED_STRING_EDITCHANNEL_ID_TITLE);
				break;
			case EEditChannelItemDescription:
				SetTitleL(R_LOCALIZED_STRING_EDITCHANNEL_DESCRIPTION_TITLE);
				break;
			default:;
		}
		
		CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
	
		// Reset title
		SetTitleL(iTitleResourceId);
		
		StoreSettingsL();
		
		if(!iChannelItem && ((aIdentifier == EEditChannelItemTitle && iId.Length() == 0) || aIdentifier == EEditChannelItemId)) {
			// Check for availablility
			iChannelSaveAllowed = false;
			
			if(aIdentifier == EEditChannelItemTitle) {
				iId.Copy(iTitle);
			}
			
			CXmppChannelIdUtilities::ValidateId(iId);
			
			LoadSettingsL();
			
			if(iId.Length() > 0) {
				_LIT(KRootNode, "/channel/");
				HBufC* aNodeId = HBufC::NewLC(KRootNode().Length() + iId.Length());
				TPtr pNodeId(aNodeId->Des());
				pNodeId.Append(KRootNode);
				pNodeId.Append(iId);
				
				CollectChannelMetadataL(pNodeId);
				CleanupStack::PopAndDestroy(); // aNodeId
			}
			
			ListBox()->SetCurrentItemIndexAndDraw(EEditChannelItemId - 1);
		}
		
		DrawDeferred();
	}
}

CAknSettingItem* CBuddycloudEditChannelList::CreateSettingItemL (TInt aIdentifier) {
	CAknSettingItem* aSettingItem = NULL;

    switch(aIdentifier) {
		case EEditChannelItemTitle:
			aSettingItem = new (ELeave) CAknTextSettingItem(aIdentifier, iTitle);
			aSettingItem->SetSettingPageFlags(CAknTextSettingPage::EPredictiveTextEntryPermitted);
			break;
		case EEditChannelItemId:
			aSettingItem = new (ELeave) CAknTextSettingItem(aIdentifier, iId);
			aSettingItem->SetSettingPageFlags(CAknTextSettingPage::EPredictiveTextEntryPermitted);
			
			if(iChannelItem) {
				aSettingItem->SetHidden(true);
			}
			break;
		case EEditChannelItemDescription:
			aSettingItem = new (ELeave) CAknTextSettingItem(aIdentifier, iDescription);
			aSettingItem->SetSettingPageFlags(CAknTextSettingPage::EZeroLengthAllowed | CAknTextSettingPage::EPredictiveTextEntryPermitted);
			break;
		case EEditChannelItemAccess:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iAccess);
			
			if(iChannelItem && iChannelItem->GetItemType() != EItemChannel) {
				aSettingItem->SetHidden(true);
			}
			break;
		case EEditChannelItemAffiliation:
			aSettingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, iAffiliation);
			
			if(!iChannelItem) {
				aSettingItem->SetHidden(true);
			}
		default:
			break;
    }

    return aSettingItem;
}

void CBuddycloudEditChannelList::XmppStanzaAcknowledgedL(const TDesC8& aStanza, const TDesC8& /*aId*/) {
	CXmlParser* aXmlParser = CXmlParser::NewLC(aStanza);
	
	if(aXmlParser->MoveToElement(_L8("item-not-found"))) {
		// Channel id not found
		iChannelSaveAllowed = true;		
	}
	else {
		if(iChannelItem) {
			// Process metadata of channel
			while(aXmlParser->MoveToElement(_L8("field"))) {
				TPtrC8 aAttributeVar(aXmlParser->GetStringAttribute(_L8("var")));
				
				if(aXmlParser->MoveToElement(_L8("value"))) {
					TPtrC aEncDataValue(iTextUtilities->Utf8ToUnicodeL(aXmlParser->GetStringData()));							
					
					if(aAttributeVar.Compare(_L8("pubsub#title")) == 0) {
						iTitle.Copy(iTextUtilities->Utf8ToUnicodeL(aXmlParser->GetStringData()).Left(iTitle.MaxLength()));
					}
					else if(aAttributeVar.Compare(_L8("pubsub#description")) == 0) {
						iDescription.Copy(iTextUtilities->Utf8ToUnicodeL(aXmlParser->GetStringData()).Left(iDescription.MaxLength()));
						
						iTextUtilities->FindAndReplace(iDescription, TChar(10), TChar(8233));
					}
					else if(aAttributeVar.Compare(_L8("pubsub#access_model")) == 0) {
						iAccess = CXmppEnumerationConverter::PubsubAccessModel(aXmlParser->GetStringData());
					}
					else if(aAttributeVar.Compare(_L8("pubsub#default_affiliation")) == 0) {
						iAffiliation = CXmppEnumerationConverter::PubsubAffiliation(aXmlParser->GetStringData());
						
						// Convert affiliation
						iAffiliation = (iAffiliation == EPubsubAffiliationMember ? 0 : 1);
					}
				}
			}
			
			iChannelSaveAllowed = true;
			iChannelEditAllowed = true;
			
			LoadSettingsL();		
			DrawDeferred();
		}
		else {
			// Existing channel found
			HBufC* aMessage = CEikonEnv::Static()->AllocReadResourceLC(R_LOCALIZED_STRING_NOTE_CHANNELEXISTS);
			CAknErrorNote* aDialog = new (ELeave) CAknErrorNote(true);		
			aDialog->ExecuteLD(*aMessage);
			CleanupStack::PopAndDestroy(); // aMessage
		}
	}
	
	CleanupStack::PopAndDestroy(); // aXmlParser
}
