/*
* ==============================================================================
*  Name        : wlanmgmtinterface.h
*  Part of     : WLAN Engine / Adaptation
*  Interface   : 
*  Description : ECom interface definition for WLAN management services.
*  Version     : %version: 7 %
*
*  Copyright � 2002-2005 Nokia. All rights reserved.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia. All rights are reserved. Copying, including 
*  reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia.
* ==============================================================================
*/

#ifndef WLANMGMTINTERFACE_H
#define WLANMGMTINTERFACE_H

// INCLUDES
#include "wlanmgmtcommon.h"
#include "wlanscaninfo.h"

// CLASS DECLARATION
/**
 * @brief ECom interface class for WLAN management services.
 *
 * This class contains the methods for managing WLAN connections
 * and querying the statuses of various connection variables.
 * @since Series 60 3.0
 */
class MWlanMgmtInterface
    {
    public:

        /**
         * Activate the notification service.
         * 
         * After the client has enabled the notification service, it can
         * receive asynchronous notifications from the server.
         * @param aCallback The class that implements the callback interface.
         */
        virtual void ActivateNotificationsL(
            MWlanMgmtNotifications& aCallback ) = 0;
        
        /**
         * Cancel the notification service.
         */
        virtual void CancelNotifications() = 0;

        /**
         * Perform a scan and return the detected WLAN networks.
         * @param aResults Results of the scan.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetScanResults(
            CWlanScanInfo& aResults ) = 0;

        /**
         * Perform a scan and return the detected WLAN networks.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        virtual void GetScanResults(
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults ) = 0;

        /**
         * Get the BSSID of the BSS currently connected to.
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aBssid BSSID of the currently connected BSS.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionBssid(
            TWlanBssid& aBssid ) = 0;

        /**
         * Get the SSID of the WLAN network currently connected to.
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSsid SSID of the currently connected network.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSsid(
            TWlanSsid& aSsid ) = 0;

        /**
         * Get the current Received Signal Strength Indicator (RSSI).
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aSignalQuality Current RSSI.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSignalQuality(
            TInt32& aSignalQuality ) = 0;

        /**
         * Get the mode of the WLAN connection.
         * @param aMode The current mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionMode(
            TWlanConnectionMode& aMode ) = 0;

        /**
         * Get the currently used security mode of the WLAN connection.
         * @remark This method can only be used while successfully connected to
         *         a WLAN network.
         * @param aMode The security mode of the connection.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetConnectionSecurityMode(
            TWlanConnectionSecurityMode& aMode ) = 0;
        
        /**
         * Get the available WLAN IAPs.
         * @param aAvailableIaps Array of IAP IDs available.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt GetAvailableIaps(
            RArray<TUint>& aAvailableIaps ) = 0;
        
        /**
         * Get the available WLAN IAPs.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.       
         * @param aAvailableIaps Array of IAP IDs available.
         */
        virtual void GetAvailableIaps(
            TRequestStatus& aStatus,
            RArray<TUint>& aAvailableIaps ) = 0;

        /**
         * Notify the server about changed WLAN settings.
         */
        virtual void NotifyChangedSettings() = 0;

        /**
         * Adds a bssid to the blacklist
         * @param aBssid The BSSID of the accesspoint.
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt AddBssidToBlacklist(
            const TWlanBssid& aBssid ) = 0;

        /**
         * Updates the RSS notification class boundaries.
         * @param aRssLevelBoundary specifies the RSS level when notification
         *     should be given. This boundary is used when signal level is
         *     getting worse (see next parameter).
         * @param aHysteresis specifies the difference between RSS notification
         *     trigger levels of declining and improving signal quality.
         *     I.e. since aRssLevelBoundary specifies the level boundary for
         *     declining signal, the same boundary for imrpoving signal is
         *     ( aRssLevelBoundary - aHysteresis ).
         * @return KErrNone if successful, otherwise one of the system-wide
         *         error codes.
         */
        virtual TInt UpdateRssNotificationBoundary( 
            const TInt32 aRssLevelBoundary,
            const TInt32 aHysteresis ) = 0;

        /**
         * Perform a direct scan for an SSID and return the detected WLAN networks.
         * @param aSsid name of the wlan network
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         *
         * Note: By giving an empty (zero length) descriptor as an aSsid leads to
         * regular broadcast scan.
         */
        virtual void GetScanResults(
            TWlanSsid& aSsid,
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults ) = 0;
        
        /**
         * Start Protected Setup.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aId Service ID of network which user has selected to be configured.
         * @param aCredentials Results of a successful Protected Setup operation.
         * @sa \link psetup Protected Setup-specific error codes \endlink.
         */
        virtual void RunProtectedSetup(
            TRequestStatus& aStatus,
            TUint32 aId,
            CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials ) = 0;

        /**
         * Cancel Protected Setup.
         */
        virtual void CancelProtectedSetup() = 0;
    };

// WLANMGMTINTERFACE_H
#endif
            
// End of File
