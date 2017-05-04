#include "hwutils.h"
#include "../stdafx.h"
#include <wininet.h>
#include <cstring>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <urlmon.h>
#include <ctype.h>
#include <stdio.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <sstream>
#include <string>
extern bool Unhook_Cheat;

#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "urlmon.lib")

std::string ReadHWFile()
{
	std::ifstream ifs( "C:\Windows\o.txt" );
	std::string content( ( std::istreambuf_iterator< char >( ifs ) ),
	                     ( std::istreambuf_iterator< char >() ) );
	return content;
}

/*char* getMAC();

char* getMAC() {
stringstream ss;
string GETMAC;
PIP_ADAPTER_INFO AdapterInfo;
DWORD dwBufLen = sizeof(AdapterInfo);
char *mac_addr = (char*)malloc(17);

AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
if (AdapterInfo == NULL) {
}

// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
if (AdapterInfo == NULL) {
}
}

if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
do {
sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
pAdapterInfo->Address[0], pAdapterInfo->Address[1],
pAdapterInfo->Address[2], pAdapterInfo->Address[3],
pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
return mac_addr;

printf("\n");
pAdapterInfo = pAdapterInfo->Next;
} while (pAdapterInfo);
}
free(AdapterInfo);
}
*/
/*
string serial() {
DWORD  lVolSerialNbr = 0;
char sHDSerial[255] = "";
//nSize = sizeof(cUsername);
GetVolumeInformation("C:\\", 0, 0, &lVolSerialNbr, 0, 0, 0, 0);
_ultoa(lVolSerialNbr, sHDSerial, 10);
std::string c(sHDSerial);
return c;
}
*/
/*
int encrypt(float Xv, int Bv) {
int x = Xv;
int B = 3; Bv % 21;
//int y = (((((((x) ^ 2)*2.7 - 44) + B*(x / 32.3) * 81 + cos(x) - sin(x ^ 3 + B)*B*x) / (421)) - x)*(2.3)*sin(x) + 2 * cos(x)*cos(x) - (B*x) / 3 + cos(x) * 13)*10;
int y = (x * 3);
return abs(y);
}
*/
//bool ajakeschecks() {
/*
IStream* pStream = NULL;
string var1 = ReadHWFile();
string var2 = serial();
int Xv = rand() % 40000;
int Bv = (atoi(var2.c_str()));
int encrypts = encrypt(Xv, Bv);

string var3 = "1";
string website = "http://dingosquad.net/antic.php?var1=";
website.append(var1).append("&var2=").append(var2).append("&var3=").append(var3).append("&var4=").append(to_string(Xv));
LPCSTR lpcstr = website.c_str();
//cout << website << endl;
if (SUCCEEDED(URLOpenBlockingStream(0, lpcstr, &pStream, 0, 0))) {
pStream->Release();
HINTERNET connect = InternetOpen("MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

HINTERNET OpenAddress = InternetOpenUrl(connect, lpcstr, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

if (!OpenAddress) {
InternetCloseHandle(connect);
return true;
}
char DataReceived[100];
DWORD NumberOfBytesRead = 0;
while (InternetReadFile(OpenAddress, DataReceived, 100, &NumberOfBytesRead) && NumberOfBytesRead) {}

InternetCloseHandle(OpenAddress);
InternetCloseHandle(connect);
//cout << "DR " << DataReceived << endl;
stringstream value;
for (int i = 0; i < 20; i++) {
if (DataReceived[i] == '0' || DataReceived[i] == '1' || DataReceived[i] == '2' || DataReceived[i] == '3' || DataReceived[i] == '4' || DataReceived[i] == '5' || DataReceived[i] == '6' || DataReceived[i] == '7' || DataReceived[i] == '8' || DataReceived[i] == '9')
value << DataReceived[i];
}
string encc(value.str());
int encryptedint;
value >> encryptedint;
//cout << encryptedint << endl;
int decrypted = encryptedint;
//cout << "De: " << (decrypted) << " Ekey: " << encrypts << endl;
if (strstr(DataReceived, "7767")) {
return true;
}
else {
Unhook_Cheat = true;
return false;

}


}
else {
return true;

}
return true;
*/
//}
bool skeltal()
{
	//return true;
	HINTERNET connect = InternetOpen( "MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0 );

	if( !connect )
		return false;

	std::string URL = "http://ausboost.net/skel1.php";
	LPCSTR Final = URL.c_str();

	HINTERNET OpenAddress = InternetOpenUrl( connect, Final, nullptr, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0 );

	if( !OpenAddress )
	{
		InternetCloseHandle( connect );
		return false;
	}

	char DataReceived[4096];
	DWORD NumberOfBytesRead = 0;
	while( InternetReadFile( OpenAddress, DataReceived, 4096, &NumberOfBytesRead ) && NumberOfBytesRead )
	{
	}

	InternetCloseHandle( OpenAddress );
	InternetCloseHandle( connect );

	if( strstr( DataReceived, "1" ) )
	{
		return true;
	}

	return false;
}

/*
bool DoesHWIDMatch() {
return true;
//stringstream ss;
//string GETMAC;
//ss << getMAC;
//ss >> GETMAC;
HINTERNET connect = InternetOpen("MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

if (!connect)
return false;

std::string URL = "http://ausboost.net/test888/newcheckuser.php?var3=" + ReadHWFile();
LPCSTR Final = URL.c_str();

HINTERNET OpenAddress = InternetOpenUrl(connect, Final, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

if (!OpenAddress) {
InternetCloseHandle(connect);
return false;
}

char DataReceived[4096];
DWORD NumberOfBytesRead = 0;
while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead) {}

InternetCloseHandle(OpenAddress);
InternetCloseHandle(connect);

if (strstr(DataReceived, "1")) {
return true;
}

return false;
}
*/
