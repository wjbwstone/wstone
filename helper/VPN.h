#ifndef _HELPER_VPN_2017_12_01_16_03_56_H
#define _HELPER_VPN_2017_12_01_16_03_56_H

/*******************************************************************
*@brief  : 如果弹出了网络选择，请使用
//snqu::CreateRegistryKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Network\\NetworkLocationWizard");
//snqu::SetRegistryDwordValueA(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Network\\NetworkLocationWizard","HideWizard",1);
********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Iphlpapi.h>

#include <Ras.h>
#include <memory>

#define ERROR_INVALID_SIZE 632
#define MAX_KEY_LENGTH 1024

#pragma comment(lib, "Rasapi32.lib")
#pragma comment(lib, "Iphlpapi.lib")

static const char* s_phonebook = "xyaoyao.PBK";
//static const char* s_phonebook = nullptr;

class VPN {
public:
	static bool createVPN(
		const char* vpnname, 
		const char* servername, 
		const char* username, 
		const char* pwd) {
		RASENTRYA rasEntry;
		memset(&rasEntry, 0, sizeof(rasEntry));
		rasEntry.dwDialExtraPercent = 75;
		rasEntry.dwDialExtraSampleSeconds = 120;
		rasEntry.dwDialMode = RASEDM_DialAll;
		rasEntry.dwType = RASET_Vpn;
		rasEntry.dwRedialCount = 30;
		rasEntry.dwRedialPause = 60;
		rasEntry.dwSize = sizeof(rasEntry);
		rasEntry.dwFramingProtocol = RASFP_Ppp;
		rasEntry.dwVpnStrategy = VS_L2tpOnly;
		rasEntry.dwfNetProtocols = RASNP_Ip;
		rasEntry.dwEncryptionType = ET_Optional;
		rasEntry.dwHangUpExtraPercent = 10;
		rasEntry.dwHangUpExtraSampleSeconds = 120;
		rasEntry.dwfOptions = RASEO_SwCompression | 
			RASEO_UseLogonCredentials |
			RASEO_RequireEncryptedPw | 
			RASEO_RequireDataEncryption | 
			RASEO_UseCountryAndAreaCodes;
		strcpy(rasEntry.szLocalPhoneNumber, servername);
		strcpy(rasEntry.szDeviceType, "vpn");
		strcpy(rasEntry.szDeviceName, "xiaoyaoVPN");

		switch(RasValidateEntryNameA(s_phonebook, vpnname)) {
		case ERROR_SUCCESS:
			break;
		case ERROR_INVALID_NAME:
			printf("Entry name: %s is invalid\n", vpnname); return false;
		case ERROR_ALREADY_EXISTS:
			printf("Entry name: %s already exists in the default phone book\n", vpnname);
			break;
		default:
			printf("RasValidateEntryName failed: Error = %d\n", GetLastError());
			return false;
		}

		if(0 != RasSetEntryPropertiesA(s_phonebook, 
			vpnname, &rasEntry, sizeof(rasEntry), nullptr, 0)) {
			return false;
		}

		RASDIALPARAMSA rdParams;
		ZeroMemory(&rdParams, sizeof(RASDIALPARAMSA));
		rdParams.dwSize = sizeof(RASDIALPARAMSA);
		strcpy(rdParams.szEntryName, vpnname);
		strcpy(rdParams.szUserName, username);
		strcpy(rdParams.szPassword, pwd);
		auto code = RasSetEntryDialParamsA(s_phonebook, &rdParams, true);
		if(ERROR_SUCCESS == code) {
			return true;
		}

#if (WINVER >= 0x601)
		if(code == ERROR_INVALID_SIZE) {
			rdParams.dwSize = offsetof(RASDIALPARAMSA, dwIfIndex);
			code = RasSetEntryDialParamsA(s_phonebook, &rdParams, true);
		}
#elif (WINVER >= 0x401)
		if (code == ERROR_INVALID_SIZE) {
			ras_param.dwSize = offsetof(RASDIALPARAMSA, dwSubEntry);
			code = RasSetEntryDialParamsA(s_phonebook, &rdParams, true);
		}
#endif

		if(ERROR_SUCCESS == code) {
			return true;
		}

		printf("RasSetEntryDialParams failed: Error = %d\n", code);
		return false;
	}

	static bool deleteVPN(const char* vpnname) {
		if(nullptr != vpnname) {
			return RasDeleteEntryA(s_phonebook, vpnname) ? false : true;
		}

		return false;
	}

	static HRASCONN connnectVPN(
		const char* vpnname, 
		const char* servername, 
		const char* username, 
		const char* pwd) {
		RASDIALPARAMSA ras_param;
		ZeroMemory(&ras_param, sizeof(ras_param));
		ras_param.dwSize = sizeof(ras_param);
		strcpy(ras_param.szEntryName, vpnname);
		strcpy(ras_param.szPhoneNumber, servername);
		strcpy(ras_param.szUserName, username);
		strcpy(ras_param.szPassword, pwd); 

#if (WINVER >= 0x601)
		ras_param.dwSize = offsetof(RASDIALPARAMSA, dwIfIndex);
#elif (WINVER >= 0x401)
		ras_param.dwSize = offsetof(RASDIALPARAMSA, dwSubEntry);
#endif

		DWORD ret = 0;
		HRASCONN hRasConn = nullptr;
		if(0 != (ret = RasDialA(NULL, s_phonebook, &ras_param, 
			0, &RasDialFunc, &hRasConn))) {
			OutputString("RasDial %s failed with error %d\n", 
				ras_param.szPhoneNumber, ret);
			RasDeleteEntryA(s_phonebook, ras_param.szEntryName);
			return nullptr;
		}

		return hRasConn;
	}

	static bool disconnectVPN(HRASCONN h) {
		return RasHangUpA(h) ? false : true;
	}

	static void WINAPI RasDialFunc(UINT unMsg, 
		RASCONNSTATE rasconnstate, DWORD dwError) {
		char szRasString[1024] = {0};
		if(dwError) {
			RasGetErrorStringA((UINT)dwError, szRasString, 1023);
			OutputString("Error: %d - %s\n",dwError, szRasString);
			char szErr[256] = {0};
			sprintf_s(szErr,"Error:%d - %s\n",dwError,szRasString);
			return;
		}

		switch(rasconnstate) {
		case RASCS_OpenPort:
			OutputString ("Opening port...\n");
			break;
		case RASCS_PortOpened:
			OutputString ("Port opened.\n");
			break;
		case RASCS_ConnectDevice: 
			OutputString ("Connecting device...\n");
			break;
		case RASCS_DeviceConnected: 
			OutputString ("Device connected.\n");
			break;
		case RASCS_AllDevicesConnected:
			OutputString ("All devices connected.\n");
			break;
		case RASCS_Authenticate: 
			OutputString ("Authenticating...\n");
			break;
		case RASCS_AuthNotify:
			OutputString ("Authentication notify.\n");
			break;
		case RASCS_AuthRetry: 
			OutputString ("Retrying authentication...\n");
			break;
		case RASCS_AuthCallback:
			OutputString ("Authentication callback...\n");
			break;
		case RASCS_AuthChangePassword: 
			OutputString ("Change password...\n");
			break;
		case RASCS_AuthProject: 
			OutputString ("Projection phase started...\n");
			break;
		case RASCS_AuthLinkSpeed: 
			OutputString ("Negotiating speed...\n");
			break;
		case RASCS_AuthAck: 
			OutputString ("Authentication acknowledge...\n");
			break;
		case RASCS_ReAuthenticate: 
			OutputString ("Retrying Authentication...\n");
			break;
		case RASCS_Authenticated: 
			OutputString ("Authentication complete.\n");
			break;
		case RASCS_PrepareForCallback: 
			OutputString ("Preparing for callback...\n");
			break;
		case RASCS_WaitForModemReset: 
			OutputString ("Waiting for modem reset...\n");
			break;
		case RASCS_WaitForCallback:
			OutputString ("Waiting for callback...\n");
			break;
		case RASCS_Projected:  
			OutputString ("Projection completed.\n");
			break;
#if (WINVER >= 0x400) 
		case RASCS_StartAuthentication:
			OutputString ("Starting authentication...\n");
			break;
		case RASCS_CallbackComplete: 
			OutputString ("Callback complete.\n");
			break;
		case RASCS_LogonNetwork:
			OutputString ("Logon to the network.\n");
			break;
#endif 
		case RASCS_SubEntryConnected:
			OutputString ("Subentry connected.\n");
			break;
		case RASCS_SubEntryDisconnected:
			OutputString ("Subentry disconnected.\n");
			break;
		case RASCS_Connected: {
				OutputString ("连接成功\n");
				auto viip = VPN::getVPNInterface("xiaoyaoVPN");
				OutputString("vpn网络接口卡地址 ： %s\n", viip.c_str());
			}
			break;
		case RASCS_Disconnected: 
			OutputString ("Disconnecting...\n");
			break;
		default:
			OutputString ("Unknown Status = %d\n", rasconnstate);
			break;
		}
	}

	/*******************************************************************
	*@brief  : 获取VPN网络IP地址
	********************************************************************/
	static std::string getVPNInterface(const char* vpnname) {
		if(nullptr == vpnname) {
			return std::string("");
		}

		DWORD dwRetVal;                 
		DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);  
		auto pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);  
		if(ERROR_SUCCESS != GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) ){  
			free(pAdapterInfo);  
			pAdapterInfo =(IP_ADAPTER_INFO *)malloc(ulOutBufLen);  
		}

		if(ERROR_SUCCESS != (dwRetVal = 
			GetAdaptersInfo(pAdapterInfo, &ulOutBufLen))){  
			OutputString("GetAdaptersInfo Error! &d\n", dwRetVal);  
			return false;
		}  

		std::string ret;
		auto pAdapter = pAdapterInfo;  
		while(pAdapter) {
			if(0 != strcmp(pAdapter->Description, vpnname)) {
				pAdapter = pAdapter->Next;
				continue;
			}

			ret = pAdapter->IpAddressList.IpAddress.String;
			break;
		}  

		if(pAdapterInfo){  
			free(pAdapterInfo);  
		}  

		return ret;  
	}  

	static void OutputString(char *lpFmt, ... ) {
		char buff[1024] = "";
		va_list arglist;
		va_start( arglist, lpFmt );
		vsnprintf( buff, 1023, lpFmt, arglist );
		va_end( arglist );
		OutputDebugStringA(buff);
		printf("%s\n", buff);
	}

	static bool IsWow64() {
		typedef BOOL (WINAPI *P)(HANDLE, PBOOL);

		auto ret = FALSE;
		auto f = (P)GetProcAddress(GetModuleHandleA("kernel32"), 
			"IsWow64Process");
		if(NULL != f) {
			f(GetCurrentProcess(),&ret);
		}

		return ret?true:false;
	}

	static void QueryKey(HKEY hKey, const char* name) { 
		DWORD    cbName;                  
		DWORD    cSubKeys=0;              
		DWORD    cbMaxSubKey;             
		DWORD    cValues;        
		DWORD    cchMaxValue;    
		DWORD    cbMaxValueData;
		CHAR    achKey[MAX_KEY_LENGTH];  

		auto retCode = RegQueryInfoKey(hKey, nullptr, 0, nullptr,
			&cSubKeys, &cbMaxSubKey, nullptr, &cValues, &cchMaxValue,           
			&cbMaxValueData, nullptr, nullptr);      

		if(0 == cSubKeys) {
			return;
		}

		for(DWORD i=0; i<cSubKeys; i++) { 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyExA(hKey, i, achKey, 
				&cbName, nullptr, nullptr, nullptr, nullptr); 
			if(retCode != ERROR_SUCCESS) {
				continue;
			}

			HKEY subkey = nullptr;
			auto ret = RegOpenKeyExA(hKey, achKey, 0, KEY_READ, &subkey);
			if(nullptr == subkey) {
				continue;
			}

			char desc[256] = {0};
			DWORD len = 255;
			DWORD type = REG_SZ;
			ret = RegGetValueA(subkey, nullptr, "Description", 
				RRF_RT_REG_SZ, &type, desc, &len);
			std::string tname = desc;
			if(std::string::npos != tname.find(name)) {
				RegDeleteKeyExA(hKey, achKey, KEY_SET_VALUE, 0);
			}

			RegCloseKey(subkey);
			subkey = nullptr;
		}
	}

	static bool ClearVPNCache(const char* vpnname) {
		if(nullptr == vpnname) {
			return false;
		}

		auto dw = KEY_WOW64_64KEY;
		if(!IsWow64()) {
			dw = 0;
		}

		HKEY hKey = 0;
		auto ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkList\\Profiles", 
			0, KEY_ALL_ACCESS | dw, &hKey);
		if(ret != ERROR_SUCCESS) {
			return false;
		}
		QueryKey(hKey, vpnname);

		ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkList\\Signatures\\Unmanaged", 
			0, KEY_ALL_ACCESS | dw, &hKey);
		if(ret != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}

		QueryKey(hKey, vpnname);
		RegCloseKey(hKey);
		return true;
	}
};


#endif//_HELPER_VPN_2017_12_01_16_03_56_H