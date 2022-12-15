#ifndef CONFIG_H
#define CONFIG_H

struct Config
{
    char *expr = nullptr;
    double point = 0;
    int diff_order = 0;
    int decompose_order = 0;
    double decompose_point = 0;
    double tangent_point = 0;
    double x_min = 0;
    double x_max = 0;
    double y_min = 0;
    double y_max = 0;
};

#endif /* CONFIG_H */
