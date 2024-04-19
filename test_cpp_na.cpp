#define _WIN32_WINNT 0x0600
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

int main(){
  const int working_buffer_size = 15000;
  const int max_tries = 3;

  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

  // default to unspecified address family (both)
  ULONG family = AF_UNSPEC;

  ULONG outBufLen = working_buffer_size;
  ULONG Iterations = 0;
  DWORD dwRetVal = 0;
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;

  // successively allocate the required memory until GetAdaptersAddresses does not 
  // results in ERROR_BUFFER_OVERFLOW for a maximum of max_tries
  do{

    pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
    if (pAddresses == NULL) {
      std::cerr << "Memory allocation failed for IP_ADAPTER_ADDRESSES struct" << std::endl;
      return 0;
    }

    dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

  } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < max_tries));

  if (dwRetVal == NO_ERROR) {
    PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    unsigned int i = 0;
    while (pCurrAddresses) {
      printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);
      pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++){
                  if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
                  {
                      std::cout << "\t\t IPV4 : ";
                      sockaddr_in *si = (sockaddr_in *)(pUnicast->Address.lpSockaddr);
                      char a[INET_ADDRSTRLEN];
                      inet_ntop(AF_INET, &(si->sin_addr), a, sizeof(a));
                      std::cout << a << std::endl;
                  }
                  else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
                  {
                      std::cout << "\t\t IPV6 : ";
                      sockaddr_in6 *si = (sockaddr_in6 *)(pUnicast->Address.lpSockaddr);
                      char a[INET6_ADDRSTRLEN];
                      inet_ntop(AF_INET6, &(si->sin6_addr), a, sizeof(a));
                      std::cout << a << std::endl;
                  }

                  pUnicast = pUnicast->Next;
                    }
            } 
            printf("\n");
      pCurrAddresses = pCurrAddresses->Next;
    }
  } else {
    std::cerr << "Call to GetAdaptersAddresses failed with error: "<< dwRetVal << std::endl;
  }

  if (pAddresses) free(pAddresses);

  return 0;
}

