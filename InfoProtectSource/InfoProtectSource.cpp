#include <Windows.h>
#include <stdio.h>
#include <AccCtrl.h>
#include <aclapi.h>
#include <winnt.h>

int main()
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		printf("OpenProcessToken Error %d", GetLastError());
		return 1;
	}


	DWORD dwSize = 0;
	BOOL bResult = GetTokenInformation(hToken, TokenUser, NULL, dwSize, &dwSize);
	if (!bResult)
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			printf("Error size GetTokenInformation! %d", GetLastError());
			return 1;
		}
	}

	PTOKEN_USER tu = (PTOKEN_USER)LocalAlloc(LPTR, dwSize);
	// Получаем маркер пользователя
	bResult = GetTokenInformation(hToken, TokenUser, tu, dwSize, &dwSize);
	if (!bResult)
	{
		printf("Error Token User GetTokenInformation! %d", GetLastError());
		return 1;
	}
	// Вытаскиваем sid из маркера пользователя
	PSID pSidCurrentUser = tu->User.Sid;
	SID_IDENTIFIER_AUTHORITY pSIDAuthWorldGroup = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSIDWorldGroup;
	bResult = AllocateAndInitializeSid(&pSIDAuthWorldGroup, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDWorldGroup);
	if (!bResult)
	{
		printf("Error WorldGroup AllocateAndInitializeSid! %d", GetLastError());
		return 1;
	}
	



	EXPLICIT_ACCESS	eaAccessList[2];
	ZeroMemory(&eaAccessList, 2 * sizeof(EXPLICIT_ACCESS));
	
	//fill Current User ACE
	eaAccessList[0].grfAccessPermissions = GENERIC_ALL;
	eaAccessList[0].grfAccessMode = SET_ACCESS;
	eaAccessList[0].grfInheritance = NO_INHERITANCE;
	eaAccessList[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	eaAccessList[0].Trustee.TrusteeType = TRUSTEE_IS_USER;
	eaAccessList[0].Trustee.ptstrName = (LPTSTR)pSidCurrentUser;

	//fill World Group ACE
	eaAccessList[1].grfAccessPermissions = GENERIC_WRITE;
	eaAccessList[1].grfAccessMode = DENY_ACCESS;
	eaAccessList[1].grfInheritance = NO_INHERITANCE;
	eaAccessList[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	eaAccessList[1].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	eaAccessList[1].Trustee.ptstrName = (LPTSTR)pSIDWorldGroup;



	PACL pAcl = NULL;
	DWORD dwResult = SetEntriesInAcl(2, eaAccessList, NULL, &pAcl);
	if (dwResult != ERROR_SUCCESS)
	{
		printf("Error SetEntriesInAcl! %d", GetLastError());
		return 1;
	}

	PSECURITY_DESCRIPTOR pSD;
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (pSD == NULL)
	{
		printf("Error LocalAlloc! %d", GetLastError());
		return 1;
	}

	bResult = InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
	if (!bResult)
	{
		printf("Error InitializeSecurityDescriptor! %d", GetLastError());
		return 1;
	}

	bResult = SetSecurityDescriptorDacl(pSD, TRUE, pAcl, FALSE);	
	if (!bResult)
	{
		printf("Error SetSecurityDescriptorDacl! %d", GetLastError());
		return 1;
	}

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;

	CreateFile(L"testfile.txt", NULL, 0, &sa, CREATE_ALWAYS, 0, NULL);

	FreeSid(pSIDWorldGroup);
	LocalFree(pAcl);
	LocalFree(pSD);
	LocalFree(tu);
	return 0;
}
