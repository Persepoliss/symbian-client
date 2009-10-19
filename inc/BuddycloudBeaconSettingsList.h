/*
============================================================================
 Name        : BuddycloudBeaconSettingsList.h
 Author      : Ross Savage
 Copyright   : Buddycloud 2008
 Description : Declares Settings List
============================================================================
*/

#ifndef BUDDYCLOUDBEACONSETTINGSLIST_H
#define BUDDYCLOUDBEACONSETTINGSLIST_H

// INCLUDES
#include <aknnavi.h>
#include <coecntrl.h>
#include <aknsettingitemlist.h>
#include "BuddycloudBeaconSettingsList.h"
#include "BuddycloudLogic.h"

// CLASS DECLARATION
class CBuddycloudBeaconSettingsList : public CAknSettingItemList {
	public: // Constructors and destructor
		CBuddycloudBeaconSettingsList(CBuddycloudLogic* aBuddycloudLogic);
		void ConstructL(const TRect& aRect);
		~CBuddycloudBeaconSettingsList();
		
	public:
		void EditCurrentItemL();
		
	public: // From CCoeControl
		void GetHelpContext(TCoeHelpContext& aContext) const;

	private: // From CCoeControl
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	public: // From CAknSettingItemList
		void EditItemL(TInt aIndex, TBool aCalledFromMenu);
		
	private:
		CAknSettingItem* CreateSettingItemL (TInt aSettingId);

	private: // Data	
		CAknNavigationControlContainer* iNaviPane;
		CAknNavigationDecorator* iNaviDecorator;

		CBuddycloudLogic* iBuddycloudLogic;
		
		TBool iWlanValue;
		TBool iGpsValue;
};

#endif

// End of File