#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <unistd.h>


//-------------------------------
//less flexibility:
typedef struct shopfloor shopfloor;
struct shopfloor
{
    char BLADE_SN[50];   //fru_serial_number (dm*)
    char ATHENA_MBPN[50];//fru_fab_pn (dm*)
    char KSB_SN_1[50];   //fru_vendor_serial (ksb*/1)
    char KSB_SN_2[50];   //fru_vendor_serial (ksb*/2)
    char YB_KSB_PN[50];  //fru_fab_pn (ksb*/1)
    char YB_KSB_PN2[50]; //fru_fab_pn (ksb*/2)
}shopfloor_array[10];

typedef struct dm dm;
struct dm
{
    char fru_serial_number[50];
    char fru_vendor_serial[50];
    char dm_fru_fab_pn[50];
    char ksb1_fru_vendor_serial[50];
    char ksb1_fru_fab_pn[50];
    char ksb2_fru_vendor_serial[50];
    char ksb2_fru_fab_pn[50];
}dm_array[10];
//-------------------------------

int main()
{
    FILE* fru_log = fopen(".\\data\\516-21060100400041_FRU.LOG", "r");

    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)))
    //     printf(cwd);

    if (!fru_log)
    {
        printf("File not exist!\n");
        system("pause");
        return 1;
    }

    char buffer[50];
    size_t str_length = 0;
    unsigned int index = 0;
    unsigned int dm_start = 0;
    unsigned int ksb_start = 0;
    unsigned int ksb_switch = 1;
    
    while (fgets(buffer, sizeof(buffer), fru_log))
    {
        // DM-------------------------------------------------------
        if (strstr(buffer, "========== FRU data of ABB =========="))
        {
            dm_start = 1;
        }
        else if (dm_start && strstr(buffer, "fru_serial_number: "))
        {
            str_length = strlen(buffer + 21) - 1;
            strcpy(dm_array[index].fru_serial_number, buffer + 21);
            dm_array[index].fru_serial_number[str_length] = '\0';
            
        }
        else if (dm_start && strstr(buffer, "fru_fab_pn: "))
        {
            str_length = strlen(buffer + 14) - 1;
            strcpy(dm_array[index].dm_fru_fab_pn, buffer + 14);
            dm_array[index].dm_fru_fab_pn[str_length] = '\0';
        }
        else if (dm_start && strstr(buffer, "fru_vendor_serial: "))
        {
            str_length = strlen(buffer + 21) - 1;
            strcpy(dm_array[index].fru_vendor_serial, buffer + 21);
            dm_array[index].fru_vendor_serial[str_length] = '\0';
            ++index;
        }

        // KSB-------------------------------------------------------
        else if (strstr(buffer, "========== FRU data of KSB =========="))
        {
            ksb_start = 1;
            dm_start = 0;
            index = 0;
        }
        else if (ksb_start && strstr(buffer, "fru_fab_pn: "))
        {
            str_length = strlen(buffer + 14) - 1;
            if (ksb_switch)
            {
                strcpy(dm_array[index].ksb1_fru_fab_pn, buffer + 14);
                dm_array[index].ksb1_fru_fab_pn[str_length] = '\0';
            }
            else
            {
                strcpy(dm_array[index].ksb2_fru_fab_pn, buffer + 14);
                dm_array[index].ksb2_fru_fab_pn[str_length] = '\0';
            }
        }
        else if (ksb_start && strstr(buffer, "fru_vendor_serial: "))
        {
            str_length = strlen(buffer + 21) - 1;
            if (ksb_switch)
            {
                strcpy(dm_array[index].ksb1_fru_vendor_serial, buffer + 21);
                dm_array[index].ksb1_fru_vendor_serial[str_length] = '\0';
                ksb_switch = 0;
                continue;
            }
            else
            {
                strcpy(dm_array[index].ksb2_fru_vendor_serial, buffer + 21);
                dm_array[index].ksb2_fru_vendor_serial[str_length] = '\0';
                ksb_switch = 1;
            }
            ++index;
        }
    }

    // for (int i = 0; i < 10; ++i)
    // {
    //     printf("fru_serial_number: %s\n", dm_array[i].fru_serial_number);
    //     printf("fru_vendor_serial: %s\n", dm_array[i].fru_vendor_serial);
    //     printf("ksb1_fru_vendor_serial: %s\n", dm_array[i].ksb1_fru_vendor_serial);
    //     printf("ksb2_fru_vendor_serial: %s\n", dm_array[i].ksb2_fru_vendor_serial);
    // }

    fclose(fru_log);
    
    char shopfloor_path[100];
    for (index = 0; index < 10; ++index)
    {
        strcpy(shopfloor_path, ".\\data\\LOG\\");
        strcat(shopfloor_path, dm_array[index].fru_vendor_serial);
        strcat(shopfloor_path, ".txt");
        // printf(shopfloor_path);

        FILE* shopfloor_log = fopen(shopfloor_path, "r");
        
        if (!shopfloor_log)
        {
            printf("File not exist!\n");
            system("pause");
            return 1;
        }
        
        while (fgets(buffer, sizeof(buffer), shopfloor_log))
        {
            if (strstr(buffer, "YB BLADE SN"))
            {
                str_length = strlen(buffer + 25) - 1;
                strcpy(shopfloor_array[index].BLADE_SN, buffer + 25);
                shopfloor_array[index].BLADE_SN[str_length] = '\0';
            }
            else if (strstr(buffer, "KSB SN2"))
            {
                str_length = strlen(buffer + 8) - 1;
                strcpy(shopfloor_array[index].KSB_SN_2, buffer + 8);
                shopfloor_array[index].KSB_SN_2[str_length] = '\0';
            }
            else if (strstr(buffer, "KSB SN"))
            {
                str_length = strlen(buffer + 7) - 1;
                strcpy(shopfloor_array[index].KSB_SN_1, buffer + 7);
                shopfloor_array[index].KSB_SN_1[str_length] = '\0';
            }
            else if (strstr(buffer, "ATHENA MBPN"))
            {
                str_length = strlen(buffer + 12) - 1;
                memcpy(shopfloor_array[index].ATHENA_MBPN, buffer + 12, str_length - 3);
                strcat(shopfloor_array[index].ATHENA_MBPN, " R");
                strcat(shopfloor_array[index].ATHENA_MBPN, buffer + 23);
                shopfloor_array[index].ATHENA_MBPN[str_length + 1] = '\0';
            }
            else if (strstr(buffer, "YB KSB PN2"))
            {
                str_length = strlen(buffer + 11) - 1;
                memcpy(shopfloor_array[index].YB_KSB_PN2, buffer + 11, str_length - 3);
                strcat(shopfloor_array[index].YB_KSB_PN2, " R");
                strcat(shopfloor_array[index].YB_KSB_PN2, buffer + 22);
                shopfloor_array[index].YB_KSB_PN2[str_length + 1] = '\0';
            }
            else if (strstr(buffer, "YB KSB PN"))
            {
                str_length = strlen(buffer + 10) - 1;
                memcpy(shopfloor_array[index].YB_KSB_PN, buffer + 10, str_length - 3);
                strcat(shopfloor_array[index].YB_KSB_PN, " R");
                strcat(shopfloor_array[index].YB_KSB_PN, buffer + 21);
                shopfloor_array[index].YB_KSB_PN[str_length + 1] = '\0';
            }
        }

        fclose(shopfloor_log);
    }
    
    // for (int i = 0; i < 10; ++i)
    // {
    //     printf("fru_serial_number: %s\n", shopfloor_array[i].BLADE_SN);
    //     printf("ksb1_fru_vendor_serial: %s\n", shopfloor_array[i].KSB_SN_1);
    //     printf("ksb2_fru_vendor_serial: %s\n", shopfloor_array[i].KSB_SN_2);
    // }

    for (index = 0; index < 10; ++index)
    {
        if (strcmp(dm_array[index].fru_serial_number, shopfloor_array[index].BLADE_SN))
        {
            printf("----------Difference!----------\n");
            printf("dm%-2d      fru_serial_number = %s\n", index+1, dm_array[index].fru_serial_number);
            printf("shopfloor YB BLADE SN       = %s\n\n", shopfloor_array[index].BLADE_SN);            
        }
        if (strcmp(dm_array[index].dm_fru_fab_pn, shopfloor_array[index].ATHENA_MBPN))
        {
            printf("----------Difference!----------\n");
            printf("dm%-2d      fru_fab_pn        = %s\n", index+1, dm_array[index].dm_fru_fab_pn);
            printf("shopfloor ATHENA MBPN       = %s\n\n", shopfloor_array[index].ATHENA_MBPN);
        }
        if (strcmp(dm_array[index].ksb1_fru_vendor_serial, shopfloor_array[index].KSB_SN_1))
        {
            printf("----------Difference!----------\n");
            printf("ksb%-2d/1   fru_vendor_serial = %s\n", index+1, dm_array[index].ksb1_fru_vendor_serial);
            printf("shopfloor KSB SN            = %s\n\n", shopfloor_array[index].KSB_SN_1);
        }
        if (strcmp(dm_array[index].ksb2_fru_vendor_serial, shopfloor_array[index].KSB_SN_2))
        {
            printf("----------Difference!----------\n");
            printf("ksb%-2d/2   fru_vendor_serial = %s\n", index+1, dm_array[index].ksb2_fru_vendor_serial);
            printf("shopfloor KSB SN2           = %s\n\n", shopfloor_array[index].KSB_SN_2);
        }
        if (strcmp(dm_array[index].ksb1_fru_fab_pn, shopfloor_array[index].YB_KSB_PN))
        {
            printf("----------Difference!----------\n");
            printf("ksb%-2d/1   fru_fab_pn        = %s\n", index+1, dm_array[index].ksb1_fru_fab_pn);
            printf("shopfloor YB KSB PN         = %s\n\n", shopfloor_array[index].YB_KSB_PN);
        }
        if (strcmp(dm_array[index].ksb2_fru_fab_pn, shopfloor_array[index].YB_KSB_PN2))
        {
            printf("----------Difference!----------\n");
            printf("ksb%-2d/2   fru_fab_pn        = %s\n", index+1, dm_array[index].ksb2_fru_fab_pn);
            printf("shopfloor YB KSB PN2        = %s\n\n", shopfloor_array[index].YB_KSB_PN2);
        }
    }
    
    system("pause");
    return 0;
}