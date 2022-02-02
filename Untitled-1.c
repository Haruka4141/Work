#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <unistd.h>

// typedef struct fru_data_struct fru_data;
// struct fru_data_struct
// {
//     char* name;
//     char* data;
//     fru_data* next;
// }fru_data_default = {NULL, NULL, NULL};

size_t fru_data_name_count = 14;
enum fru_data_name
{
    fru_assy_pn,
    fru_fab_pn,
    fru_mac_address,
    fru_mfg_check_point,
    fru_mfg_info_ver,
    fru_mfg_location,
    fru_mfg_week,
    fru_mfg_year,
    fru_misc,
    fru_serial_number,
    fru_status,
    fru_type_code,
    fru_type_str,
    fru_vendor_serial
};

// size_t fru_data_name_PSU_count = 16;
// enum fru_data_name_PSU
// {
//     fru_status,
//     pmbus_mfr_date,
//     pmbus_mfr_id,
//     pmbus_mfr_iout_max_A,
//     pmbus_mfr_location,
//     pmbus_mfr_max_temp_1_C,
//     pmbus_mfr_max_temp_2_C,
//     pmbus_mfr_model,
//     pmbus_mfr_pin_max_W,
//     pmbus_mfr_pout_max_W,
//     pmbus_mfr_revision,
//     pmbus_mfr_serial,
//     pmbus_mfr_tambient_max_C,
//     pmbus_mfr_tambient_min_C,
//     pmbus_mfr_vin_max_V,
//     pmbus_mfr_vin_min_V
// };

enum shopfloor_name
{
    BLADE_SN,   //fru_serial_number (dm*)
    KSB_SN_1,   //fru_vendor_serial(ksb*/1)
    KSB_SN_2,   //fru_vendor_serial(ksb*/2)
};

typedef struct component_struct component;
struct component_struct
{
    char* FRU_type;
    char* component_name;
    char** fru_data;
    component* next;
}/*component_struct_default={NULL, NULL, NULL, NULL}*/;

struct dm
{
    component* dm;
    component* ksb1;
    component* ksb2;
}dm[10];

int main()
{
    FILE* fru_log = fopen(".\\data\\516-21060100400041_FRU.LOG", "r");

    char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)))
    //     printf(cwd);

    if (!fru_log)
    {
        printf("File not exist!\n");
        system("pause");
        return 1;
    }

    char buffer[100];
    char fru_type_buffer[20];
    component comp_dummy_head;
    component* comp_current = &comp_dummy_head;
    while (fgets(buffer, sizeof(buffer), fru_log))
    {
        if (strstr(buffer, "========== FRU data of " ))
        {
            size_t fru_type_length = strcspn(buffer+23, " ");
            memcpy(fru_type_buffer, buffer+23, fru_type_length);
            fru_type_buffer[fru_type_length] = '\0';
            // printf(fru_type_buffer);
        }
        if (strstr(buffer, "component: "))
        {
            comp_current->next = malloc(sizeof(component));
            comp_current = comp_current->next;
            comp_current->next = NULL;

            comp_current->FRU_type = malloc(strlen(fru_type_buffer)+1);
            memcpy(comp_current->FRU_type, fru_type_buffer, sizeof(comp_current->FRU_type));

            // printf("scanned!\n");
            size_t component_name_length = strlen(buffer+11);                   // '\n'
            comp_current->component_name = malloc(component_name_length);
            strcpy(comp_current->component_name, buffer+11);
            comp_current->component_name[component_name_length-1] = '\0';

            comp_current->fru_data = malloc(sizeof(char*) * fru_data_name_count);
            for (int i = 0; i < fru_data_name_count; ++i)
            {
                fgets(buffer, sizeof(buffer), fru_log);
                size_t data_start_index = strcspn(buffer, ":") + 2;
                size_t data_length = strlen(buffer) - data_start_index - 1;     // '\n'
                comp_current->fru_data[i] = malloc(data_length);
                strcpy(comp_current->fru_data[i], buffer + data_start_index);
                (comp_current->fru_data[i])[data_length] = '\0';
                //printf(comp_current->fru_data[i]);
            }
        }
    }

    // component* curr = comp_dummy_head.next;
    // while(curr)
    // {
    //     for (int i = 0; i < fru_data_name_count; ++i)
    //         printf("%s\n", curr->fru_data[i]);
    //     curr = curr->next;
    // }


    fclose(fru_log);
    component* curr = comp_dummy_head.next;



    //FILE* Shopfloor_log = fopen("..\\data\\516-21060100600136.txt", "r");


    system("pause");
    return 0;
}