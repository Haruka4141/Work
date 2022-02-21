#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

// Warning: original string will be modified!
char* get_token(char* buffer, char* delimiters, int get_index)
{
    char* token;
    int index = 0;
    token = strtok(buffer, delimiters);         // Notice that this string is modified by being broken into smaller strings (tokens).
    ++index;

    while(token && index <= get_index)
    {
        token = strtok(NULL, delimiters);       // A null pointer may be specified, in which case the function continues scanning where a previous successful call to the function ended.
        ++index;
    }

    return token;                           
}

// delete '-' add " R" in the end of PN
void PN_transform(char* token)
{            
    memcpy( token + strlen(token) - 1, token + strlen(token) - 2, 3);
    token[strlen(token) - 4] = ' ';
    token[strlen(token) - 3] = 'R'; 
}

int main()
{
    // check fru & shopfloor file exists or not
    char fru_log_path[100];
    char shopfloor_path[100];
    printf("Please enter fru log file path: ");
    scanf("%s", &fru_log_path);
    FILE* fru_log = fopen(fru_log_path, "r");
    if (!fru_log)
    {
        printf("%s\nFru log file does not exist!\n", fru_log_path);
        system("pause");
        return 1;
    }
    printf("Please enter shopfloor data file path: ");
    scanf("%s", &shopfloor_path);
    FILE* shopfloor_log = fopen(shopfloor_path, "r");   // Caution! using UTF-8 for fgets()!
    if (!shopfloor_log)
    {
        printf("Shopfloor data file does not exist!\n", shopfloor_path);
        system("pause");
        return 1;
    }

    char buffer[500];
    size_t str_length = 0;
    unsigned int index = 0;
    unsigned int dm_start = 0;
    unsigned int ksb_start = 0;
    unsigned int ksb_switch = 1;

    // Collect node & ksb data from .LOG
    while (fgets(buffer, sizeof(buffer), fru_log))
    {
        // Node-------------------------------------------------------
        if (strstr(buffer, "========== FRU data of ABB =========="))
        {
            dm_start = 1;
        }
        else if (dm_start && strstr(buffer, "fru_serial_number"))
        {
            strcpy(dm_array[index].fru_serial_number, get_token(buffer, ": \n", 1));
        }
        else if (dm_start && strstr(buffer, "fru_fab_pn"))      //get_token is different
        {
            strcpy(dm_array[index].dm_fru_fab_pn, get_token(buffer, ":\n", 1) + 1);            
        }
        else if (dm_start && strstr(buffer, "fru_vendor_serial: "))
        {
            strcpy(dm_array[index].fru_vendor_serial, get_token(buffer, ": \n", 1));
            ++index;                //vendor serial is the last item
        }

        // Ksb-------------------------------------------------------
        else if (strstr(buffer, "========== FRU data of KSB =========="))
        {
            ksb_start = 1;
            dm_start = 0;
            index = 0;
        }
        else if (ksb_start && strstr(buffer, "fru_fab_pn"))     //get_token is different
        {
            if (ksb_switch)
            {
                strcpy(dm_array[index].ksb1_fru_fab_pn, get_token(buffer, ":\n", 1) + 1);
            }
            else
            {
                strcpy(dm_array[index].ksb2_fru_fab_pn, get_token(buffer, ":\n", 1) + 1);         
            }
        }
        else if (ksb_start && strstr(buffer, "fru_vendor_serial"))
        {
            if (ksb_switch)
            {
                strcpy(dm_array[index].ksb1_fru_vendor_serial, get_token(buffer, ": \n", 1));  
                ksb_switch = 0;     //switching between ksb 1/2
                continue;
            }
            else
            {
                strcpy(dm_array[index].ksb2_fru_vendor_serial, get_token(buffer, ": \n", 1));  
                ksb_switch = 1;     //switching between ksb 1/2
            }
            ++index;                //vendor serial is last item
        }
    }

    // for (int i = 0; i < 10; ++i)
    // {
    //     printf("dm%d\n", i+1);
    //     printf("fru_serial_number: %s\n", dm_array[i].fru_serial_number);
    //     printf("fru_vendor_serial: %s\n", dm_array[i].fru_vendor_serial);
    //     printf("ksb1_fru_vendor_serial: %s\n", dm_array[i].ksb1_fru_vendor_serial);
    //     printf("ksb2_fru_vendor_serial: %s\n", dm_array[i].ksb2_fru_vendor_serial);
    // }

    fclose(fru_log);

    // collect ksb data in shop floor
    char dm_ksb_link[50];
    int shopfloor_node_exist[10] = {0};                     // 1 if node exist in shopfloor
    fgets(buffer, sizeof(buffer), shopfloor_log);           // skip first row (title)
    while (fgets(buffer, sizeof(buffer), shopfloor_log))
    {   
        strcpy(dm_ksb_link, get_token(buffer, "\t\n", 0));
        buffer[strlen(buffer)] = '\t';                      // reset original string after using get_token
        for(index = 0; index < 10; ++index)                 // C doesn't have dict, using linear search
        {
            if(strcmp(dm_ksb_link, dm_array[index].fru_vendor_serial) == 0)
            {    
                shopfloor_node_exist[index] = 1;
                break;
            }
        }
        if (index >= 10)    // shop floor does not exist
            continue;

        if (strstr(buffer, "YB BLADE SN"))
        {                                                                       // vv: barcode right part
            strcpy(shopfloor_array[index].BLADE_SN, get_token(buffer, "\t\n", 2) + 13);
        }
        else if (strstr(buffer, "KSB SN2"))         //because of strstr(), longer token must check earlier
        {
            strcpy(shopfloor_array[index].KSB_SN_2, get_token(buffer, "\t\n", 2));
        }
        else if (strstr(buffer, "KSB SN"))
        {
            strcpy(shopfloor_array[index].KSB_SN_1, get_token(buffer, "\t\n", 2));  
        }
        else if (strstr(buffer, "ATHENA MBPN"))     //PN transform
        {
            strcpy(shopfloor_array[index].ATHENA_MBPN, get_token(buffer, "\t\n", 2));     
            PN_transform(shopfloor_array[index].ATHENA_MBPN);
        }
        else if (strstr(buffer, "YB KSB PN2"))      //because of strstr(), longer token must check earlier
        {
            strcpy(shopfloor_array[index].YB_KSB_PN2, get_token(buffer, "\t\n", 2));     
            PN_transform(shopfloor_array[index].YB_KSB_PN2);        //PN transform 
        }
        else if (strstr(buffer, "YB KSB PN"))       //PN transform
        {
            strcpy(shopfloor_array[index].YB_KSB_PN, get_token(buffer, "\t\n", 2));     
            PN_transform(shopfloor_array[index].YB_KSB_PN);
        }
    }
    
    // for (int i = 0; i < 10; ++i)
    // {
    //     printf("node%d\n", i+1);
    //     printf("fru_serial_number: %s\n", shopfloor_array[i].BLADE_SN);
    //     printf("ksb1_fru_vendor_serial: %s\n", shopfloor_array[i].KSB_SN_1);
    //     printf("ksb2_fru_vendor_serial: %s\n", shopfloor_array[i].KSB_SN_2);
    // }

    printf("\n\n");

    // transfer filename extension: .log -> _compare.txt
    fru_log_path[strlen(fru_log_path) - 4] = '\0';
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
        if (shopfloor_node_exist[index] == 0)
        {
            sprintf(buffer, "shop floor data: Node %d does not exist!  (Barcode: %s)\n\n", index + 1, dm_array[index].fru_vendor_serial);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf("%s", buffer);
            continue;
        }
        if (strcmp(dm_array[index].fru_serial_number, shopfloor_array[index].BLADE_SN))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "dm%-2d      fru_serial_number = %s\n"
                            "shopfloor YB BLADE SN       = %s\n\n", 
                            index+1, dm_array[index].fru_serial_number, shopfloor_array[index].BLADE_SN);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);

        }
        if (strcmp(dm_array[index].dm_fru_fab_pn, shopfloor_array[index].ATHENA_MBPN))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "dm%-2d      fru_fab_pn        = %s\n"
                            "shopfloor ATHENA MBPN       = %s\n\n", 
                            index+1, dm_array[index].dm_fru_fab_pn, shopfloor_array[index].ATHENA_MBPN);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);
        }
        if (strcmp(dm_array[index].ksb1_fru_vendor_serial, shopfloor_array[index].KSB_SN_1))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "ksb%-2d/1   fru_vendor_serial = %s\n"
                            "shopfloor KSB SN            = %s\n\n", 
                            index+1, dm_array[index].ksb1_fru_vendor_serial, shopfloor_array[index].KSB_SN_1);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);
        }
        if (strcmp(dm_array[index].ksb2_fru_vendor_serial, shopfloor_array[index].KSB_SN_2))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "ksb%-2d/2   fru_vendor_serial = %s\n"
                            "shopfloor KSB SN2           = %s\n\n", 
                            index+1, dm_array[index].ksb2_fru_vendor_serial, shopfloor_array[index].KSB_SN_2);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);
        }
        if (strcmp(dm_array[index].ksb1_fru_fab_pn, shopfloor_array[index].YB_KSB_PN))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "ksb%-2d/1   fru_fab_pn        = %s\n"
                            "shopfloor YB KSB PN         = %s\n\n", 
                            index+1, dm_array[index].ksb1_fru_fab_pn, shopfloor_array[index].YB_KSB_PN);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);
        }
        if (strcmp(dm_array[index].ksb2_fru_fab_pn, shopfloor_array[index].YB_KSB_PN2))
        {
            sprintf(buffer, "----------Difference!----------\n"
                            "ksb%-2d/2   fru_fab_pn        = %s\n"
                            "shopfloor YB KSB PN2        = %s\n\n", 
                            index+1, dm_array[index].ksb2_fru_fab_pn, shopfloor_array[index].YB_KSB_PN2);
            if (output_log)
                fwrite(buffer, strlen(buffer), 1, output_log);
            printf(buffer);
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
            printf("SN Check Fail!\n");
            printf("Log is recorded to %s\n", fru_log_path);
        }
    }
    printf("\n\n");
    system("pause");
    return 0;
}