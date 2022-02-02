#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <unistd.h>

typedef struct fru_data
{
    char* fru_assy_pn;
    char* fru_fab_pn;
    char* fru_mac_address;
    char* fru_mfg_check_point;
    char* fru_mfg_info_ver;
    char* fru_mfg_location;
    char* fru_mfg_week;
    char* fru_mfg_year;
    char* fru_misc;
    char* fru_serial_number;
    char* fru_status;
    char* fru_type_code;
    char* fru_type_str;
    char* fru_vendor_serial
}fru_data;

typedef struct fru_data_PSU
{
    char* fru_status;
    char* pmbus_mfr_date;
    char* pmbus_mfr_id;
    char* pmbus_mfr_iout_max_A;
    char* pmbus_mfr_location;
    char* pmbus_mfr_max_temp_1_C;
    char* pmbus_mfr_max_temp_2_C;
    char* pmbus_mfr_model;
    char* pmbus_mfr_pin_max_W;
    char* pmbus_mfr_pout_max_W;
    char* pmbus_mfr_revision;
    char* pmbus_mfr_serial;
    char* pmbus_mfr_tambient_max_C;
    char* pmbus_mfr_tambient_min_C;
    char* pmbus_mfr_vin_max_V;
    char* pmbus_mfr_vin_min_V;
}fru_data_PSU;

struct component
{
    char* FRU_type;
    char* component_name;
    fru_data data;
    struct component* next;
};

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
    struct component comp_dummy_head;
    struct component* comp_current = &comp_dummy_head;
    while (fgets(buffer, sizeof(buffer), fru_log))
    {
        if (strstr(buffer, "========== FRU data of " ))
        {
            size_t fru_type_length = strcspn(buffer+23, " ");
            memcpy(fru_type_buffer, buffer+23, fru_type_length);
            fru_type_buffer[fru_type_length] = '\0';
            // printf(fru_type_buffer);
            comp_current->next = malloc(sizeof(struct component));
            comp_current = comp_current->next;
        }
        if (strstr(buffer, "component:"))
        {
            // printf("scanned!\n");
            comp_current->FRU_type = malloc(strlen(fru_type_buffer)*sizeof(char)+1);
            memcpy(comp_current->FRU_type, fru_type_buffer, strlen(buffer)*sizeof(char)+1);
            fgets(buffer, sizeof(buffer), fru_log);
            // printf(buffer);
            comp_current->data.fru_assy_pn = malloc(strlen(buffer)*sizeof(char)+1);
            memcpy(comp_current->data.fru_assy_pn, buffer, strlen(buffer)*sizeof(char)+1);
            printf(comp_current->data.fru_assy_pn);

        }
    }

    system("pause");
    return 0;
}