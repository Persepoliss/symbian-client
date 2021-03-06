/*
============================================================================
 Name        : 	BuddycloudLogic.h
 Author      : 	Ross Savage
 Copyright   : 	2007 Buddycloud
 Description : 	Access for Buddycloud Client UI to Buddycloud low level
 				operations
 History     : 	1.0

		1.0	Initial Development
============================================================================
*/

#ifndef BUDDYCLOUDLOGIC_H_
#define BUDDYCLOUDLOGIC_H_

#include <e32base.h>
#include <flogger.h>
#include "Buddycloud.hrh"
#include "AvatarRepository.h"
#include "BuddycloudConstants.h"
#include "BuddycloudFollowing.h"
#include "BuddycloudPlaces.h"
#include "BuddycloudNearby.h"
#include "DiscussionManager.h"
#include "LocationEngine.h"
#include "NotificationEngine.h"
#include "PhoneUtilities.h"
#include "Timer.h"
#include "TextUtilities.h"
#include "XmppEngine.h"
#include "XmppInterfaces.h"

/*
----------------------------------------------------------------------------
--
-- Enumerations
--
----------------------------------------------------------------------------
*/

enum TDescSettingItems {
	ESettingItemFullName, ESettingItemUsername, ESettingItemPassword, ESettingItemServer,
	ESettingItemPrivateMessageToneFile, ESettingItemChannelPostToneFile, ESettingItemDirectReplyToneFile,
	ESettingItemTwitterUsername, ESettingItemTwitterPassword
};

enum TIntSettingItems {
	ESettingItemNotifyChannelsFollowing = ESettingItemTwitterPassword + 1, ESettingItemNotifyChannelsModerating, 
	ESettingItemChannelPostTone, ESettingItemDirectReplyTone, ESettingItemPrivateMessageTone, ESettingItemLanguage
};

enum TBoolSettingItems {
	ESettingItemCellOn = ESettingItemLanguage + 1, ESettingItemWifiOn, ESettingItemGpsOn, 
	ESettingItemCellAvailable, ESettingItemWifiAvailable, ESettingItemGpsAvailable, 
	ESettingItemNewInstall, ESettingItemNotifyReplyTo, ESettingItemAutoStart, ESettingItemAccessPoint, 
	ESettingItemMessageBlocking, ESettingItemMarkAllChannelsRead
};

enum TBuddycloudLogicState {
	ELogicShutdown, ELogicOffline, ELogicConnecting, ELogicOnline
};

enum TBuddycloudLogicNotificationType {
	ENotificationLogicEngineStarted, ENotificationLogicEngineShuttingDown, ENotificationLogicEngineDestroyed, 
	ENotificationFollowingItemsUpdated, ENotificationFollowingItemsReconfigured, ENotificationFollowingItemDeleted, 
	ENotificationPlaceItemsUpdated, ENotificationLocationUpdated, ENotificationNotifiedMessageEvent, 
	ENotificationEditPlaceRequested, ENotificationEditPlaceCompleted, ENotificationActivityChanged, 
	ENotificationConnectivityChanged, ENotificationAuthenticationFailed, ENotificationServerResolveFailed
};

enum TBuddycloudLogicTimeoutState {
	ETimeoutNone, ETimeoutStartConnection, ETimeoutConnected, ETimeoutSaveSettings, ETimeoutSavePlaces
};

enum TBuddycloudToneType {
	EToneDefault, EToneNone, EToneUserDefined
};

enum TBuddycloudCustomLanguages {
	ELangPirate = 512, ELangBoarisch, ELangLOLspeak
};

/*
----------------------------------------------------------------------------
--
-- Interfaces
--
----------------------------------------------------------------------------
*/

class MBuddycloudLogicNotificationObserver {
	public:
		virtual void NotificationEvent(TBuddycloudLogicNotificationType aEvent, TInt aId = KErrNotFound) = 0;
};

class MBuddycloudLogicStatusObserver {
	public:
		virtual void JumpToItem(TInt aItemId) = 0;
};

class MBuddycloudLogicOwnerObserver {
	public:
		virtual void StateChanged(TBuddycloudLogicState aState) = 0;
		virtual void LanguageChanged(TInt aLanguage) = 0;
		virtual void ShutdownComplete() = 0;
};

class MBuddycloudLogicDebugObserver {
	public:
		virtual void StanzaSent(const TDesC8& aMessage) = 0;
		virtual void StanzaReceived(const TDesC8& aMessage) = 0;
};

/*
----------------------------------------------------------------------------
--
-- CBuddycloudLogic
--
----------------------------------------------------------------------------
*/

class CBuddycloudLogic : public CBase, MLocationEngineNotification, MTimeInterface,
									MXmppEngineObserver, MXmppRosterObserver, MXmppStanzaObserver,
									MTimeoutNotification, MAvatarRepositoryObserver,
									MDiscussionReadObserver {

	public:
		CBuddycloudLogic(MBuddycloudLogicOwnerObserver* aObserver);
		static CBuddycloudLogic* NewL(MBuddycloudLogicOwnerObserver* aObserver);
		~CBuddycloudLogic();

	public:
		void Startup();
		void PrepareShutdown();

	private:
		void ConstructL();

	public: // XMPP connection
		void ConnectL();
		void Disconnect();
		
		TBuddycloudLogicState GetState();
		
		MXmppWriteInterface* GetXmppInterface();
		
		TInt GetNewIdStamp();
		
		void GetConnectionStatistics(TInt& aDataSent, TInt& aDataReceived);

	private:
		void ConnectToXmppServerL();
		
		void SendPresenceL();
		void AddRosterManagementItemL(const TDesC8& aJid);
		void SendPresenceSubscriptionL(const TDesC8& aTo, const TDesC8& aType, const TDesC8& aOptionalData = KNullDesC8);
		
	public: // Settings
		void ResetConnectionSettings();
		void ValidateUsername(TBool aCheckServer = true);
		
		TDes& GetDescSetting(TDescSettingItems aItem);
		TInt& GetIntSetting(TIntSettingItems aItem);
		TBool& GetBoolSetting(TBoolSettingItems aItem);
		
		void SettingsItemChanged(TInt aSettingsItemId);
		
	private:
		void ResetStoredDataL();
		
	public: // Activity status
		TDesC& GetActivityStatus();
		void SetActivityStatus(const TDesC& aText);
	
	private: 
		void SetActivityStatus(TInt aResourceId);
		void ShowInformationDialogL(TInt aResourceId);
		
	public:
		TXmppPubsubAffiliation ShowAffiliationDialogL(const TDesC& aNode, const TDesC& aJid, TXmppPubsubAffiliation aAffiliation, TBool aNotifyResult = false);

	private:
		TInt DisplaySingleLinePopupMenuL(RPointerArray<HBufC>& aMenuItems);
		TInt DisplayDoubleLinePopupMenuL(RPointerArray<HBufC>& aMenuItems);
		
//		void SendSmsOrEmailL(TDesC& aAddress, TDesC& aSubject, TDesC& aBody);
//		
//	public: // Sending to external
//		void SendInviteL(TInt aFollowerId);
//		void SendPlaceL(TInt aFollowerId);
		
	public: // Follow contact
		void FollowContactL(const TDesC& aContact);

	private: // Pubsub
		void SendPresenceToPubsubL(TInt64 aLastNodeId, TXmppMessagePriority aPriority = EXmppPriorityNormal);
		void SetLastNodeIdReceived(const TDesC8& aNodeItemId);
		
		void CollectPubsubSubscriptionsL(const TDesC8& aAfter = KNullDesC8);
		void ProcessPubsubSubscriptionsL();
		
		void CollectUsersPubsubNodeAffiliationsL(const TDesC8& aAfter = KNullDesC8);
		void ProcessUsersPubsubNodeAffiliationsL();
		
		void CollectLastPubsubNodeItemsL(const TDesC& aNode, const TDesC8& aHistoryAfterItem);		
		void CollectUserPubsubNodeL(const TDesC& aJid, const TDesC& aNodeLeaf, const TDesC8& aHistoryAfterItem = KNullDesC8);
		
		void CollectChannelMetadataL(const TDesC& aNode, TInt aStampId = EXmppIdGetChannelMetadata);
		void ProcessChannelMetadataL(const TDesC8& aStanza);
		
	public: // Pubsub
		void SetPubsubNodeAffiliationL(const TDesC& aJid, const TDesC& aNode, TXmppPubsubAffiliation aAffiliation, TBool aNotifyResult = false);
		void SetPubsubNodeAffiliationsL(const TDesC8& aNode, const TDesC8& aAffiliations);
    	
		void RequestPubsubNodeAffiliationL(const TDesC& aNode, TXmppPubsubAffiliation aAffiliation, const TDesC& aText);
		
		void SetPubsubNodeSubscriptionL(const TDesC& aJid, const TDesC& aNode, TXmppPubsubSubscription aSubscription);
		
		void RetractPubsubNodeItemL(const TDesC& aNode, const TDesC8& aNodeItemId);
		
    private: // Pubsub handling
		void HandlePubsubEventL(const TDesC8& aStanza);
		void HandlePubsubRequestL(const TDesC8& aStanza);
		
		void AcknowledgePubsubEventL(const TDesC8& aId);
		
    public: // Flag/Tag
    	void FlagTagNodeL(const TDesC8& aType, const TDesC& aNode);
    	void FlagTagNodeItemL(const TDesC8& aType, const TDesC& aNode, const TDesC8& aNodeItemId);
	
	public: // Channels
		TInt FollowChannelL(const TDesC& aNode);
		void UnfollowChannelL(TInt aItemId);
		
		TInt CreateChannelL(const TDesC& aNodeId, const TDesC& aTitle, const TDesC& aDescription, TXmppPubsubAccessModel aAccessModel);

	public: // User Status & Place
		void SetMoodL(TDesC& aMood);

		void SetCurrentPlaceL();
		void SetCurrentPlaceL(TInt aPlaceId);
		void SetNextPlaceL(TDesC& aPlace, TInt aPlaceId = KErrNotFound);

	public: // From MDiscussionReadObserver
		void DiscussionRead(TDesC& aDiscussionId, TInt aItemId);
#ifdef _DEBUG
		void DiscussionDebug(const TDesC8& aMessage);
#endif

	private: // Reading/Writing state to file
		void LoadSettingsAndItems();
		void LoadSettingsAndItemsL();
		void SaveSettingsAndItemsL();

		void LoadPlaceItems();
		void LoadPlaceItemsL();
		void SavePlaceItemsL();

		void BackupOldLog();
		void WriteToLog(const TDesC8& aText);

	public: // Server time stamp
		TTime TimeStamp();

	public: // Observers
		void AddNotificationObserver(MBuddycloudLogicNotificationObserver* aNotificationObserver);
		void RemoveNotificationObserver(MBuddycloudLogicNotificationObserver* aNotificationObserver);
		void NotifyNotificationObservers(TBuddycloudLogicNotificationType aEvent, TInt aId = KErrNotFound);

		void AddStatusObserver(MBuddycloudLogicStatusObserver* aStatusObserver);
		void RemoveStatusObserver();

	public: // Following
		CBuddycloudFollowingStore* GetFollowingStore();
		
		CFollowingRosterItem* GetOwnItem();
		void UnfollowItemL(TInt aItemId);
		
	public: // Notices
		void RespondToNoticeL(TInt aItemId, TNoticeResponse aResponse);
		
	public: // Media posting
		void MediaPostRequestL(TInt aItemId);

	public: // Messaging
		CDiscussion* GetDiscussion(const TDesC& aId);

		TInt IsSubscribedTo(const TDesC& aId, TInt aItemOptions = EItemAll);

		void PostMessageL(TInt aItemId, TDesC& aId, TDesC& aContent, const TDesC8& aReferenceId = KNullDesC8);		
		
	public: // Communities
		void SendCommunityCredentials(TCommunityItems aCommunity);

	public: // Avatar
		CAvatarRepository* GetImageRepository();

	public: // Location engine
		MLocationEngineDataInterface* GetLocationInterface();
		
	public:
		CBuddycloudPlaceStore* GetPlaceStore();

	public:
		void HandlePlaceQueryResultL(const TDesC8& aStanza, TBuddycloudXmppIdEnumerations aType);
		
	public: 
		void SendEditedPlaceDetailsL(TBool aSearchOnly);
		void GetPlaceDetailsL(TInt aPlaceId);
		void EditMyPlacesL(TInt aPlaceId, TBool aAddPlace);
	
	private:	
		void CollectPlaceSubscriptionsL();
		void ProcessPlaceSubscriptionsL(CBuddycloudPlaceStore* aPlaceStore);
		void ProcessPlaceSearchL(CBuddycloudPlaceStore* aPlaceStore);
		void SendPlaceQueryL(TInt aPlaceId, TBuddycloudXmppIdEnumerations aType, TBool aAcknowledge = false);

	public: // From MTimeoutNotification
		void TimerExpired(TInt aExpiryId);

	public: // From MAvatarRepositoryObserver
		void AvatarLoaded();

	public: // From CLocationEngine
    	void HandleLocationServerResult(TLocationMotionState aMotionState, TInt aPatternQuality, TInt aPlaceId);
 		void LocationShutdownComplete();
	
	public: // From MTimeInterface
    	TTime GetTime();

    public: // From MXmppEngineNotification
		void XmppStanzaRead(const TDesC8& aMessage);
		void XmppStanzaWritten(const TDesC8& aMessage);
		void XmppStateChanged(TXmppEngineState aState);
		void XmppUnhandledStanza(const TDesC8& aStanza);
		void XmppError(TXmppEngineError aError);
		void XmppShutdownComplete();

		void XmppDebug(const TDesC8& aMessage);

    public: // From MXmppRosterObserver
    	void XmppRosterL(const TDesC8& aStanza);
 
    private: // Roster item handling
    	void HandleUserItemConfigurationL(const TDesC& aJid);
    	void HandleRosterItemPushL(const TDesC8& aStanza);
    	
    public: // From MXmppStanzaObserver
		void XmppStanzaAcknowledgedL(const TDesC8& aStanza, const TDesC8& aId);
 
    private: // Presence handling
    	void HandleIncomingPresenceL(const TDesC8& aStanza);
       	void ProcessPresenceSubscriptionL(const TDesC8& aStanza);
   
    private: // Message handling
		void HandleIncomingMessageL(const TDesC8& aStanza);

    protected:
    	// Observers
    	MBuddycloudLogicOwnerObserver* iOwnerObserver;
		MBuddycloudLogicStatusObserver* iStatusObserver;
		RPointerArray<MBuddycloudLogicNotificationObserver> iNotificationObservers;
		
		CNotificationEngine* iNotificationEngine;

		CCustomTimer* iTimer;
		TBuddycloudLogicTimeoutState iTimerState;

		CFollowingRosterItem* iOwnItem;
		
		CTextUtilities* iTextUtilities;
		CPhoneUtilities* iPhoneUtilities;

		// Connection State
		TBuddycloudLogicState iState;
		TBool iConnectionCold;
		
		HBufC* iServerActivityText;
        HBufC* iBroadLocationText;        
        TInt64 iLastNodeIdReceived;
		
		// Internal flags
		TInt iIdStamp;
		
		TBool iRosterSynchronized;
		TBool iPubsubSubscribedTo;
		TXmppPubsubAccessModel iUserChannelAccessModel;
		
        CBuddycloudListStore* iGenericList;

		// Xmpp engine
		CXmppEngine* iXmppEngine;
        TBool iXmppEngineReady;

        // Setting Items
        TInt iSettingConnectionMode;
        TInt iSettingConnectionModeId;
        TBool iSettingNewInstall;
        
        // Positioning settings
        TBool iSettingCellOn;
        TBool iSettingWlanOn;
        TBool iSettingGpsOn;
        
        // Notifications settings
        TInt iSettingNotifyChannelsFollowing;
        TInt iSettingNotifyChannelsModerating;
        TInt iSettingPrivateMessageTone;
        TFileName iSettingPrivateMessageToneFile;
        TInt iSettingChannelPostTone;
        TFileName iSettingChannelPostToneFile;
        TInt iSettingDirectReplyTone;
        TFileName iSettingDirectReplyToneFile;
        TBool iSettingNotifyReplyTo;
        
        // Preferences settings
        TInt iSettingPreferredLanguage;
        TBool iSettingMessageBlocking;
        TBool iSettingAccessPoint;
        TBool iSettingAutoStart;
       
        // Account settings
        TBuf<32> iSettingFullName;
        TBuf<64> iSettingUsername;
        TBuf<32> iSettingPassword;
        TBuf<64> iSettingXmppServer;
        
        // Community settings
        TBuf<32> iSettingTwitterUsername;
        TBuf<32> iSettingTwitterPassword;

        // Location services
        CLocationEngine* iLocationEngine;
        TBool iLocationEngineReady;
        TLocationMotionState iLastMotionState;
        TInt iStablePlaceId;
        
        // Nearby places
        RPointerArray<CBuddycloudNearbyPlace> iNearbyPlaces;
 
		// Stores
        CBuddycloudFollowingStore* iFollowingList;
        CBuddycloudPlaceStore* iPlaceList;
        CDiscussionManager* iDiscussionManager;
       
        TInt iNextItemId;
 
        // Debug
        RFileLogger iLog;

        // Avatars
        CAvatarRepository* iAvatarRepository;
        
        // Save required
        TBool iSettingsSaveNeeded;
        TBool iPlacesSaveNeeded;
};

#endif /*BUDDYCLOUDLOGIC_H_*/
