/*
============================================================================
 Name        : BuddycloudChannelInfoContainer.h
 Author      : Ross Savage
 Copyright   : Buddycloud 2010
 Description : Declares Channel Info Container
============================================================================
*/

#ifndef BUDDYCLOUDCHANNELINFOCONTAINER_H_
#define BUDDYCLOUDCHANNELINFOCONTAINER_H_

// INCLUDES
#include "BuddycloudListComponent.h"
#include "ViewReference.h"
#include "XmppInterfaces.h"

/*
----------------------------------------------------------------------------
--
-- CBuddycloudChannelInfoContainer
--
----------------------------------------------------------------------------
*/

class CBuddycloudChannelInfoContainer : public CBuddycloudListComponent, MXmppStanzaObserver {

	public: // Constructors and destructor
		CBuddycloudChannelInfoContainer(CBuddycloudLogic* aBuddycloudLogic);
		void ConstructL(const TRect& aRect, TViewReference aQueryReference);
        ~CBuddycloudChannelInfoContainer();
	
	private: // From CBuddycloudListComponent
		void RenderWrappedText(TInt aIndex);
		TInt CalculateItemSize(TInt aIndex);
		void RenderListItems();
		void RepositionItems(TBool aSnapToItem);
		void HandleItemSelection(TInt aItemId);
		
	public:
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
		void HandleCommandL(TInt aCommand);
		
	public: // From CCoeControl
		void GetHelpContext(TCoeHelpContext& aContext) const;
		
	private: // From CCoeControl
		void SizeChanged();
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
		
    public: // From MXmppStanzaObserver
		void XmppStanzaAcknowledgedL(const TDesC8& aStanza, const TDesC8& aId);

	private: // Variables
		MXmppWriteInterface* iXmppInterface;
		TViewReference iQueryReference;
		
		TBool iDataCollected;
		
		CFollowingChannelItem* iChannelItem;
		
		HBufC* iChannelTitle;
		HBufC* iChannelDescription;
		
		RPointerArray<HBufC> iStatistics;
};

#endif /* BUDDYCLOUDCHANNELINFOCONTAINER_H_ */