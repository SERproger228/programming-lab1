#include "polynomial.h"
#include "type_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns: 1 on success, -1 on EOF, 0 on invalid input */
static int readLine(char* buffer, size_t size)
{
    if (fgets(buffer, size, stdin) == NULL) {
        return feof(stdin) ? -1 : 0;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

static int readInt(const char* prompt, int* value)
{
    char buffer[128];
    char* end = NULL;
    long parsed;
    int lineResult;

    printf("%s", prompt);
    fflush(stdout);

    lineResult = readLine(buffer, sizeof(buffer));
    if (lineResult != 1) {
        return lineResult;
    }

    parsed = strtol(buffer, &end, 10);

    if (end == buffer || *end != '\0') {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

static int readDouble(const char* prompt, double* value)
{
    char buffer[128];
    char* end = NULL;
    int lineResult;

    printf("%s", prompt);
    fflush(stdout);

    lineResult = readLine(buffer, sizeof(buffer));
    if (lineResult != 1) {
        return lineResult;
    }

    *value = strtod(buffer, &end);

    if (end == buffer || *end != '\0') {
        return 0;
    }

    return 1;
}

static int readChoice(const char* prompt, int minValue, int maxValue, int* choice)
{
    int value;
    int result = readInt(prompt, &value);

    if (result != 1) {
        return result;
    }

    if (value < minValue || value > maxValue) {
        return 0;
    }

    *choice = value;
    return 1;
}

static int readTypedValue(const TypeInfo* type, const char* prompt, void* value)
{
    if (type == getIntTypeInfo()) {
        int parsed;
        int result = readInt(prompt, &parsed);

        if (result != 1) {
            return result;
        }

        memcpy(value, &parsed, sizeof(int));
        return 1;
    }

    if (type == getDoubleTypeInfo()) {
        double parsed;
        int result = readDouble(prompt, &parsed);

        if (result != 1) {
            return result;
        }

        memcpy(value, &parsed, sizeof(double));
        return 1;
    }

    return 0;
}

static Polynomial makeEmptyPolynomial(void)
{
    Polynomial p;
    p.coefficients.data = NULL;
    p.coefficients.size = 0;
    p.coefficients.type = NULL;
    return p;
}

static Polynomial readPolynomialFromUser(const char* name, const TypeInfo* type)
{
    Polynomial polynomial;
    int degreeInput;
    size_t degree;
    size_t power;
    void* value;
    int result;

    while (1) {
        result = readInt("Enter degree: ", &degreeInput);
        if (result == -1) {
            return makeEmptyPolynomial();
        }
        if (result == 1 && degreeInput >= 0) {
            break;
        }
        printf("Invalid degree. Try again.\n");
    }

    degree = (size_t)degreeInput;
    polynomial = createPolynomial(degree, type);

    if (polynomial.coefficients.data == NULL) {
        return polynomial;
    }

    value = malloc(type->size);

    if (value == NULL) {
        freePolynomial(&polynomial);
        return polynomial;
    }

    printf("Enter coefficients for %s\n", name);

    for (power = 0; power <= degree; power++) {
        char prompt[128];

        snprintf(prompt, sizeof(prompt), "%s[%zu] = ", name, power);

        result = readTypedValue(type, prompt, value);
        if (result == -1) {
            free(value);
            freePolynomial(&polynomial);
            return makeEmptyPolynomial();
        }
        if (result == 0) {
            printf("Invalid value. Try again.\n");
            power--;
            continue;
        }

        setPolynomialCoefficient(&polynomial, power, value);
    }

    free(value);
    return polynomial;
}

static int isPolynomialReady(const Polynomial* polynomial)
{
    return polynomial != NULL &&
        polynomial->coefficients.data != NULL &&
        polynomial->coefficients.type != NULL &&
        polynomial->coefficients.size > 0;
}

static void printPolynomialLabel(const char* label, const Polynomial* polynomial)
{
    printf("%s", label);
    printPolynomial(polynomial);
    printf("\n");
}

static void printMenu(void)
{
    printf("\nMenu:\n");
    printf("1. Print P(x)\n");
    printf("2. Print Q(x)\n");
    printf("3. P(x) + Q(x)\n");
    printf("4. k * P(x)\n");
    printf("5. P(x) * Q(x)\n");
    printf("6. P(Q(x))\n");
    printf("7. P(a)\n");
    printf("8. Reenter P and Q\n");
    printf("0. Exit\n");
}

int main(void)
{
    int typeChoice;
    const TypeInfo* type;
    Polynomial p;
    Polynomial q;
    int running = 1;

    printf("Polynomial console interface\n");

    while (1) {
        int result = readChoice("Choose coefficient type (1 - int, 2 - double): ", 1, 2, &typeChoice);
        if (result == -1) {
            return 0;
        }
        if (result == 1) {
            break;
        }
        printf("Invalid choice. Try again.\n");
    }

    type = (typeChoice == 1) ? getIntTypeInfo() : getDoubleTypeInfo();

    p = readPolynomialFromUser("P", type);
    q = readPolynomialFromUser("Q", type);

    if (!isPolynomialReady(&p) || !isPolynomialReady(&q)) {
        printf("Failed to create polynomials.\n");
        freePolynomial(&p);
        freePolynomial(&q);
        return 1;
    }

    while (running) {
        int choice;

        printMenu();

        {
            int result = readChoice("Select action: ", 0, 8, &choice);
            if (result == -1) {
                break;
            }
            if (result == 0) {
                printf("Invalid choice.\n");
                continue;
            }
        }

        switch (choice) {
            case 0:
                running = 0;
                break;
            case 1:
                printPolynomialLabel("P(x) = ", &p);
                break;
            case 2:
                printPolynomialLabel("Q(x) = ", &q);
                break;
            case 3: {
                Polynomial result = addPolynomials(&p, &q);
                printPolynomialLabel("P(x) + Q(x) = ", &result);
                freePolynomial(&result);
                break;
            }
            case 4: {
                void* scalar = malloc(type->size);
                Polynomial result;

                if (scalar == NULL) {
                    printf("Memory allocation failed.\n");
                    break;
                }

                if (readTypedValue(type, "Enter scalar: ", scalar) != 1) {
                    printf("Invalid scalar.\n");
                    free(scalar);
                    break;
                }

                result = multiplyPolynomialByScalar(&p, scalar);
                printPolynomialLabel("k * P(x) = ", &result);
                freePolynomial(&result);
                free(scalar);
                break;
            }
            case 5: {
                Polynomial result = multiplyPolynomials(&p, &q);
                printPolynomialLabel("P(x) * Q(x) = ", &result);
                freePolynomial(&result);
                break;
            }
            case 6: {
                Polynomial result = composePolynomials(&p, &q);
                printPolynomialLabel("P(Q(x)) = ", &result);
                freePolynomial(&result);
                break;
            }
            case 7: {
                void* point = malloc(type->size);
                void* value = malloc(type->size);

                if (point == NULL || value == NULL) {
                    printf("Memory allocation failed.\n");
                    free(point);
                    free(value);
                    break;
                }

                if (readTypedValue(type, "Enter a: ", point) != 1) {
                    printf("Invalid point.\n");
                    free(point);
                    free(value);
                    break;
                }

                evaluatePolynomialAt(&p, point, value);

                if (type == getIntTypeInfo()) {
                    printf("P(a) = %d\n", *(int*)value);
                } else {
                    printf("P(a) = %.6f\n", *(double*)value);
                }

                free(point);
                free(value);
                break;
            }
            case 8:
                freePolynomial(&p);
                freePolynomial(&q);
                p = readPolynomialFromUser("P", type);
                q = readPolynomialFromUser("Q", type);
                break;
        }
    }

    freePolynomial(&p);
    freePolynomial(&q);

    return 0;
}
