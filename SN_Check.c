#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <unistd.h>


//-------------------------------
//less flexibility:
typedef struct shopfloor shopfloor;
struct shopfloor
{
    char BLADE_SN[50];      //fru_serial_number (dm*)
    char ATHENA_MBPN[50];   //fru_fab_pn (dm*)
    char KSB_SN_1[50];      //fru_vendor_serial (ksb*/1)
    char KSB_SN_2[50];      //fru_vendor_serial (ksb*/2)
    char YB_KSB_PN[50];     //fru_fab_pn (ksb*/1)
    char YB_KSB_PN2[50];    //fru_fab_pn (ksb*/2)
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

int main(int argc, char* argv[])
{
    FILE* fru_log = fopen(argv[1], "r");

    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)))
    //     printf(cwd);

    if (!fru_log)
    {
        printf("%s\nFru data file does not exist!\n", argv[1]);
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
        strcpy(shopfloor_path, argv[2]);
        strcat(shopfloor_path, "\\");
        strcat(shopfloor_path, dm_array[index].fru_vendor_serial);
        strcat(shopfloor_path, ".txt");
        //printf(shopfloor_path);

        FILE* shopfloor_log = fopen(shopfloor_path, "r");
        
        if (!shopfloor_log)
        {
            printf("%s\nShopfloor data file does not exist!\n", shopfloor_path);
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
            else if (strstr(buffer, "ATHENA MBPN"))     //-01/-02 <-> R01/R02 transform
            {
                str_length = strlen(buffer + 12) - 1;
                memcpy(shopfloor_array[index].ATHENA_MBPN, buffer + 12, str_length - 3);
                strcat(shopfloor_array[index].ATHENA_MBPN, " R");
                strcat(shopfloor_array[index].ATHENA_MBPN, buffer + 23);
                shopfloor_array[index].ATHENA_MBPN[str_length + 1] = '\0';
            }
            else if (strstr(buffer, "YB KSB PN2"))      //-01/-02 <-> R01/R02 transform
            {
                str_length = strlen(buffer + 11) - 1;
                memcpy(shopfloor_array[index].YB_KSB_PN2, buffer + 11, str_length - 3);
                strcat(shopfloor_array[index].YB_KSB_PN2, " R");
                strcat(shopfloor_array[index].YB_KSB_PN2, buffer + 22);
                shopfloor_array[index].YB_KSB_PN2[str_length + 1] = '\0';
            }
            else if (strstr(buffer, "YB KSB PN"))       //-01/-02 <-> R01/R02 transform
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

    char log_buffer[500];
    char fru_log_path[100];

    strcpy(fru_log_path, argv[1]);
    strstr(fru_log_path, ".LOG")[0] = '\0';
    strcat(fru_log_path, "_compare.txt");
    // printf(fru_log_path);
    
    FILE* output_log = fopen(fru_log_path, "w");
    // perror("");                                  // "w+" -> invalid arguments

    if (!output_log)
    {
        printf("Cannot create log file!\n");
    }

    // fru_log & shopfloor comparison:
    for (index = 0; index < 10; ++index)
    {        
        if (strcmp(dm_array[index].fru_serial_number, shopfloor_array[index].BLADE_SN))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "dm%-2d      fru_serial_number = %s\n"
                                 "shopfloor YB BLADE SN       = %s\n\n", 
                                 index+1, dm_array[index].fru_serial_number, shopfloor_array[index].BLADE_SN);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);

        }
        if (strcmp(dm_array[index].dm_fru_fab_pn, shopfloor_array[index].ATHENA_MBPN))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "dm%-2d      fru_fab_pn        = %s\n"
                                "shopfloor ATHENA MBPN       = %s\n\n", 
                                index+1, dm_array[index].dm_fru_fab_pn, shopfloor_array[index].ATHENA_MBPN);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);
        }
        if (strcmp(dm_array[index].ksb1_fru_vendor_serial, shopfloor_array[index].KSB_SN_1))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "ksb%-2d/1   fru_vendor_serial = %s\n"
                                "shopfloor KSB SN            = %s\n\n", 
                                index+1, dm_array[index].ksb1_fru_vendor_serial, shopfloor_array[index].KSB_SN_1);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);
        }
        if (strcmp(dm_array[index].ksb2_fru_vendor_serial, shopfloor_array[index].KSB_SN_2))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "ksb%-2d/2   fru_vendor_serial = %s\n"
                                "shopfloor KSB SN2           = %s\n\n", 
                                index+1, dm_array[index].ksb2_fru_vendor_serial, shopfloor_array[index].KSB_SN_2);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);
        }
        if (strcmp(dm_array[index].ksb1_fru_fab_pn, shopfloor_array[index].YB_KSB_PN))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "ksb%-2d/1   fru_fab_pn        = %s\n"
                                "shopfloor YB KSB PN         = %s\n\n", 
                                index+1, dm_array[index].ksb1_fru_fab_pn, shopfloor_array[index].YB_KSB_PN);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);
        }
        if (strcmp(dm_array[index].ksb2_fru_fab_pn, shopfloor_array[index].YB_KSB_PN2))
        {
            sprintf(log_buffer, "----------Difference!----------\n"
                                "ksb%-2d/2   fru_fab_pn        = %s\n"
                                "shopfloor YB KSB PN2        = %s\n\n", 
                                index+1, dm_array[index].ksb2_fru_fab_pn, shopfloor_array[index].YB_KSB_PN2);
            fwrite(log_buffer, strlen(log_buffer), 1, output_log);
            printf(log_buffer);
        }
    }
    
    if (output_log)
    {
        fseek(output_log, 0, SEEK_END);
        if (ftell(output_log) == 0)
        {
            printf("SN Check Pass!\n");
            fclose(output_log);            
            remove(fru_log_path);       // remove output if no error
            // perror("error: ");
        }
        else
        {
            fclose(output_log);
            printf("Log is recorded to %s\n", fru_log_path);
        }
    }

    system("pause");
    return 0;
}