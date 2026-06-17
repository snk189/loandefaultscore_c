#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_NAME_LEN 100

// Applicant structure
typedef struct {
    char name[MAX_NAME_LEN];
    int age;
    int education_level;
    float income;
    int employment_years;
    float loan_amount;
    
    // Dynamically populated from the multi-line history
    float total_loans_taken;
    float total_loans_paid;
} Applicant;

// Function Prototypes
int processApplicantFile(const char* txt_path, Applicant* app);
void readData(const char* filename, Applicant* app);
float calculateHighImpactScore(Applicant app);
float calculateMediumImpactScore(Applicant app);
float calculateLowImpactScore(Applicant app);
float calculateTotalRiskScore(Applicant app);

// New Native C functions
void extractTextFromPdf(const char* pdf_path, const char* out_txt_path);
void writePdf(const char* dest_path, const char* cv_text_path, const char* report_text_path);
void generateReportAndCompilePdf(Applicant app, int risk_code, const char* cv_txt_path, const char* dest_pdf_path);


void processSinglePDF(const char* filename) {
    char* ext = strrchr(filename, '.');
    if (ext == NULL || strcmp(ext, ".pdf") != 0) {
        printf("Skipping %s (Not a valid .pdf file)\n", filename);
        return;
    }
    
    char base_name[256];
    int name_len = ext - filename;
    strncpy(base_name, filename, name_len);
    base_name[name_len] = '\0';

    char pdf_path[512];
    char txt_path[512];
    sprintf(pdf_path, "pdfs/pending/%s", filename);
    sprintf(txt_path, "temp_extract.txt");

    FILE* test = fopen(pdf_path, "rb");
    if (!test) {
        printf("Error: File '%s' not found in pdfs/pending/ directory.\n", filename);
        return;
    }
    fclose(test);

    // 1. Pure C Native PDF Text Extraction!
    printf("\n---> Detected %s. Natively extracting text to temp memory...\n", filename);
    extractTextFromPdf(pdf_path, txt_path);

    // 2. Parse the TXT file and display results
    Applicant applicant;
    int risk_code = processApplicantFile(txt_path, &applicant);

    // 3. Define Output path
    const char* category_folder;
    if (risk_code == 10) category_folder = "low_risk";
    else if (risk_code == 20) category_folder = "medium_risk";
    else category_folder = "high_risk";
    
    char dest_pdf_path[512];
    sprintf(dest_pdf_path, "pdfs/done/%s/%s.pdf", category_folder, base_name);

    // 4. Generate Report and Compile Brand New PDF using Pure C
    generateReportAndCompilePdf(applicant, risk_code, txt_path, dest_pdf_path);

    // 5. Clean up temporary files & old pending PDF
    remove(txt_path);
    remove(pdf_path); // Delete original since we compiled a new one!
    
    printf("---> Natively compiled and archived %s.pdf into %s folder!\n", base_name, category_folder);
}

int main() {
    DIR *dp = opendir("pdfs/pending");

    // Fallback: If we are in the 'output' directory, step back one folder and try again!
    if (dp == NULL) {
        chdir("..");
        dp = opendir("pdfs/pending");
    }

    if (dp == NULL) {
        printf("Error: Could not find pdfs/pending directory even after checking parent folder.\n");
        return 1;
    }

    printf("\n=======================================================\n");
    printf("     AUTO-LOAN DEFAULT RISK SCORING ORCHESTRATOR       \n");
    printf("        [PURE C NATIVE PDF ENGINE COMPILED]            \n");
    printf("=======================================================\n\n");

    printf("Enter the PDF filename to process (e.g. '01_arya_cv.pdf')\n");
    printf("OR type 'all' to automatically batch-process the entire pending folder.\n");
    printf("> ");
    
    char input_file[256];
    if (scanf("%255s", input_file) != 1) return 1;

    if (strcmp(input_file, "all") == 0) {
        int pdf_count = 0;
        struct dirent *entry;

        // Iterate through all files in the pending directory
        while ((entry = readdir(dp)) != NULL) {
            char* ext = strrchr(entry->d_name, '.');
            if (ext != NULL && strcmp(ext, ".pdf") == 0) {
                pdf_count++;
                processSinglePDF(entry->d_name);
            }
        }

        closedir(dp);

        // Auto-Exit if empty
        if (pdf_count == 0) {
            printf("\nNo pending PDF applications found in pdfs/pending.\n");
        } else {
            printf("\nSuccessfully processed all %d pending applications using Pure C.\n", pdf_count);
        }
    } else {
        closedir(dp);
        // Process only the single file
        processSinglePDF(input_file);
        printf("\nDemo processing complete for %s.\n", input_file);
    }

    return 0;
}

// ==========================================
// PURE C NATIVE PDF EXTRACTOR
// ==========================================
void extractTextFromPdf(const char* pdf_path, const char* out_txt_path) {
    FILE* in = fopen(pdf_path, "rb");
    FILE* out = fopen(out_txt_path, "w");
    if (!in || !out) return;
    
    char line[1024];
    while (fgets(line, sizeof(line), in)) {
        char* start = strchr(line, '(');
        char* end = strrchr(line, ')');
        if (start && end && end > start && strstr(line, "Tj")) {
            size_t len = end - start - 1;
            char extracted[512];
            strncpy(extracted, start + 1, len);
            extracted[len] = '\0';
            fprintf(out, "%s\n", extracted);
        }
    }
    
    fclose(in);
    fclose(out);
}

// ==========================================
// PURE C NATIVE PDF COMPILER
// ==========================================
void writePdf(const char* dest_path, const char* cv_text_path, const char* report_text_path) {
    FILE* out = fopen(dest_path, "wb");
    if (!out) return;
    
    long xref[7];
    
    fprintf(out, "%%PDF-1.3\n");
    
    xref[1] = ftell(out);
    fprintf(out, "1 0 obj\n<</Type /Catalog /Pages 2 0 R>>\nendobj\n");
    
    xref[2] = ftell(out);
    fprintf(out, "2 0 obj\n<</Type /Pages /Kids [3 0 R] /Count 1>>\nendobj\n");
    
    xref[3] = ftell(out);
    fprintf(out, "3 0 obj\n<</Type /Page /Parent 2 0 R /Resources <</Font <</F1 4 0 R /F2 5 0 R>>>> /MediaBox [0 0 595.28 841.89] /Contents 6 0 R>>\nendobj\n");
    
    xref[4] = ftell(out);
    fprintf(out, "4 0 obj\n<</Type /Font /BaseFont /Helvetica /Subtype /Type1>>\nendobj\n");
    
    xref[5] = ftell(out);
    fprintf(out, "5 0 obj\n<</Type /Font /BaseFont /Helvetica-Bold /Subtype /Type1>>\nendobj\n");

    // Build the stream in memory
    char stream[20000];
    stream[0] = '\0';
    float y = 800.0f;
    
    // Title Header
    char draw_cmd[1200];
    sprintf(draw_cmd, "BT\n/F2 16 Tf\n1 0 0 1 150 %.2f Tm\n(Applicant CV / Loan Application) Tj\nET\n", y);
    strcat(stream, draw_cmd);
    y -= 30.0f;

    // Function to append text lines to the stream
    auto void append_file_to_stream(const char* filepath);
    void append_file_to_stream(const char* filepath) {
        FILE* in = fopen(filepath, "r");
        if (!in) return;
        char line[512];
        
        strcat(stream, "BT\n/F1 12 Tf\n");
        
        while (fgets(line, sizeof(line), in)) {
            // strip newline
            line[strcspn(line, "\r\n")] = '\0';
            
            // Skip the old plain-text header if it got extracted
            if (strstr(line, "Applicant CV / Loan Application") != NULL) continue;
            // Skip plain-text report header
            if (strstr(line, "OFFICIAL EVALUATION REPORT") != NULL) continue;
            if (strstr(line, "========================================") != NULL) continue;
            
            if (strlen(line) == 0) {
                y -= 15.0f;
                continue;
            }

            // basic escaping for PDF
            char escaped[1024];
            int j = 0;
            for (int i = 0; line[i] != '\0' && j < 1020; i++) {
                if (line[i] == '(' || line[i] == ')' || line[i] == '\\') {
                    escaped[j++] = '\\';
                }
                escaped[j++] = line[i];
            }
            escaped[j] = '\0';
            
            char draw_cmd2[1200];
            sprintf(draw_cmd2, "1 0 0 1 50 %.2f Tm\n(%s) Tj\n", y, escaped);
            strcat(stream, draw_cmd2);
            
            y -= 15.0f; 
        }
        strcat(stream, "ET\n");
        fclose(in);
    }
    
    append_file_to_stream(cv_text_path);
    
    y -= 25.0f; // Gap between CV and Report
    
    sprintf(draw_cmd, "BT\n/F2 14 Tf\n1 0 0 1 50 %.2f Tm\n(==================================================) Tj\nET\n", y);
    strcat(stream, draw_cmd);
    y -= 15.0f;
    sprintf(draw_cmd, "BT\n/F2 14 Tf\n1 0 0 1 180 %.2f Tm\n(OFFICIAL EVALUATION REPORT) Tj\nET\n", y);
    strcat(stream, draw_cmd);
    y -= 15.0f;
    sprintf(draw_cmd, "BT\n/F2 14 Tf\n1 0 0 1 50 %.2f Tm\n(==================================================) Tj\nET\n", y);
    strcat(stream, draw_cmd);
    y -= 20.0f;
    
    append_file_to_stream(report_text_path);
    
    xref[6] = ftell(out);
    size_t stream_len = strlen(stream);
    fprintf(out, "6 0 obj\n<</Length %zu>>\nstream\n%s\nendstream\nendobj\n", stream_len, stream);
    
    long startxref = ftell(out);
    fprintf(out, "xref\n0 7\n0000000000 65535 f \n");
    for (int i = 1; i <= 6; i++) {
        fprintf(out, "%010ld 00000 n \n", xref[i]);
    }
    
    fprintf(out, "trailer\n<< /Size 7 /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", startxref);
    fclose(out);
}

// Function to read data and return the calculated risk code
int processApplicantFile(const char* txt_path, Applicant* app) {
    readData(txt_path, app);
    
    float high = calculateHighImpactScore(*app);
    float medium = calculateMediumImpactScore(*app);
    float low = calculateLowImpactScore(*app);
    float total = calculateTotalRiskScore(*app);

    char decision[30];
    int exit_code = 0;
    
    if (total < 40.0f) {
        strcpy(decision, "LOW RISK (Approve)");
        exit_code = 10;
    }
    else if (total < 70.0f) {
        strcpy(decision, "MEDIUM RISK (Review)");
        exit_code = 20;
    }
    else {
        strcpy(decision, "HIGH RISK (Reject)");
        exit_code = 30;
    }

    printf("\nApplicant Name      : %s\n", app->name);
    printf("Age                 : %d\n", app->age);
    printf("Income              : $%.2f\n", app->income);
    printf("Loan Requested      : $%.2f\n", app->loan_amount);
    
    // Display parsed history totals
    float dynamic_debts = app->total_loans_taken - app->total_loans_paid;
    if (dynamic_debts < 0) dynamic_debts = 0;
    printf("Total Historical Debt: $%.2f (Remaining: $%.2f)\n", app->total_loans_taken, dynamic_debts);

    printf("-------------------------------------------------------\n");
    printf("High Impact Score   : %.2f / 100\n", high);
    printf("Medium Impact Score : %.2f / 100\n", medium);
    printf("Low Impact Score    : %.2f / 100\n", low);
    printf("=======================================================\n");
    printf("FINAL RISK SCORE    : %.2f / 100\n", total);
    
    return exit_code;
}

// Read data line by line
void readData(const char* filename, Applicant* app) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s.\n", filename);
        exit(1);
    }

    char line[256];
    
    // Default values
    strcpy(app->name, "Unknown");
    app->age = 0;
    app->education_level = 1;
    app->income = 0.0f;
    app->employment_years = 0;
    app->loan_amount = 0.0f;
    app->total_loans_taken = 0.0f;
    app->total_loans_paid = 0.0f;

    int parsing_history = 0;

    // Parse Key-Value pairs
    while (fgets(line, sizeof(line), file)) {
        // If we reached the history block, parse the custom format
        if (parsing_history) {
            int year;
            float loan = 0, paid = 0;
            char k1 = ' ', k2 = ' ';
            
            // Format match: "2023 100000 loan paid 20k"
            if (sscanf(line, "%d %f%c loan paid %f%c", &year, &loan, &k1, &paid, &k2) >= 4) {
                // Handle "k" or "K" suffixes for thousands
                if (k1 == 'k' || k1 == 'K') loan *= 1000.0f;
                if (k2 == 'k' || k2 == 'K') paid *= 1000.0f;
                
                app->total_loans_taken += loan;
                app->total_loans_paid += paid;
            }
            continue; // Keep looping to catch all history lines
        }

        // Trigger the history block parser when we see the header
        if (strstr(line, "Repayment history:") != NULL) {
            parsing_history = 1;
            continue;
        }

        if (sscanf(line, "Name: %99[^\r\n]", app->name) == 1) continue;
        if (sscanf(line, "Age: %d", &app->age) == 1) continue;
        if (sscanf(line, "Education: %d", &app->education_level) == 1) continue;
        if (sscanf(line, "Income: %f", &app->income) == 1) continue;
        if (sscanf(line, "Employment years: %d", &app->employment_years) == 1) continue;
        if (sscanf(line, "Loan amount: %f", &app->loan_amount) == 1) continue;
    }
    
    fclose(file);
}

float calculateHighImpactScore(Applicant app) {
    // 1. Existing Debts dynamically calculated
    float existing_debts = app.total_loans_taken - app.total_loans_paid;
    if (existing_debts < 0) existing_debts = 0;

    // 2. Repayment Risk based on percentage paid off
    float repayment_percentage = 0.0f;
    if (app.total_loans_taken > 0) {
        repayment_percentage = (app.total_loans_paid / app.total_loans_taken) * 100.0f;
    } else {
        repayment_percentage = 100.0f; // No prior loans is good!
    }

    float repaymentRisk = 100.0f - repayment_percentage;
    if (repaymentRisk < 0) repaymentRisk = 0;
    if (repaymentRisk > 100) repaymentRisk = 100;

    // 3. Debt-to-Income
    float debtRisk = 0.0f;
    if (app.income > 0) {
        float dti = existing_debts / app.income;
        debtRisk = dti * 100.0f;
        if (debtRisk > 100.0f) debtRisk = 100.0f;
    } else {
        debtRisk = 100.0f; 
    }
    
    return (repaymentRisk + debtRisk) / 2.0f;
}

float calculateMediumImpactScore(Applicant app) {
    float incomeRisk = 0.0f;
    if (app.income > 80000) incomeRisk = 10.0f;
    else if (app.income > 50000) incomeRisk = 30.0f;
    else if (app.income > 30000) incomeRisk = 60.0f;
    else incomeRisk = 90.0f;

    float empRisk = 0.0f;
    if (app.employment_years >= 5) empRisk = 10.0f;
    else if (app.employment_years >= 2) empRisk = 40.0f;
    else if (app.employment_years > 0) empRisk = 70.0f;
    else empRisk = 100.0f;

    return (incomeRisk + empRisk) / 2.0f;
}

float calculateLowImpactScore(Applicant app) {
    float ageRisk = 0.0f;
    if (app.age < 25) ageRisk = 80.0f;
    else if (app.age <= 50) ageRisk = 20.0f;
    else ageRisk = 40.0f;

    float eduRisk = 0.0f;
    if (app.education_level == 3) eduRisk = 10.0f;      
    else if (app.education_level == 2) eduRisk = 40.0f; 
    else eduRisk = 80.0f;                               

    return (ageRisk + eduRisk) / 2.0f;
}

float calculateTotalRiskScore(Applicant app) {
    float highScore = calculateHighImpactScore(app);
    float mediumScore = calculateMediumImpactScore(app);
    float lowScore = calculateLowImpactScore(app);
    return (highScore * 0.6f) + (mediumScore * 0.3f) + (lowScore * 0.1f);
}

void generateReportAndCompilePdf(Applicant app, int risk_code, const char* cv_txt_path, const char* dest_pdf_path) {
    char report_path[] = "temp_report.txt";
    
    FILE *f = fopen(report_path, "w");
    if (f == NULL) return;

    char reasons[10][100];
    int count = 1;

    // Repayment History
    float repayment_percentage = 0.0f;
    if (app.total_loans_taken > 0) {
        repayment_percentage = (app.total_loans_paid / app.total_loans_taken) * 100.0f;
    } else {
        repayment_percentage = 100.0f;
    }

    if (repayment_percentage >= 80.0f) {
        sprintf(reasons[count++], "Excellent repayment history");
    } else if (repayment_percentage >= 40.0f) {
        sprintf(reasons[count++], "Moderate repayment history");
    } else {
        sprintf(reasons[count++], "Poor repayment history");
    }

    // Income
    if (app.income >= 50000) {
        sprintf(reasons[count++], "High income");
    } else if (app.income >= 30000) {
        sprintf(reasons[count++], "Moderate income");
    } else {
        sprintf(reasons[count++], "Low income");
    }

    // Employment
    if (app.employment_years >= 3) {
        sprintf(reasons[count++], "Long employment stability");
    } else if (app.employment_years >= 1) {
        sprintf(reasons[count++], "Short employment history");
    } else {
        sprintf(reasons[count++], "Unstable employment history");
    }

    // Existing Debts / Loan Size
    float existing_debts = app.total_loans_taken - app.total_loans_paid;
    if (existing_debts < 0) existing_debts = 0;
    
    if (app.income > 0) {
        float dti = existing_debts / app.income;
        if (dti > 0.5f) {
            sprintf(reasons[count++], "High existing debt compared to income");
        } else {
            sprintf(reasons[count++], "Manageable existing debt");
        }
        
        float loan_to_income = app.loan_amount / app.income;
        if (loan_to_income > 0.5f) {
            sprintf(reasons[count++], "Requested loan amount is high compared to income");
        } else {
            sprintf(reasons[count++], "Requested loan amount is small/manageable");
        }
    } else {
        sprintf(reasons[count++], "No income to support debt");
    }

    fprintf(f, "========================================\n");
    fprintf(f, "OFFICIAL EVALUATION REPORT\n");
    fprintf(f, "========================================\n");

    // Write to console and file
    printf("\nReasons:\n");
    for (int i = 1; i < count; i++) {
        printf("Reason %d: %s\n", i, reasons[i]);
        fprintf(f, "Reason %d: %s\n", i, reasons[i]);
    }

    printf("\nRecommendation:\n");
    fprintf(f, "\nRecommendation:\n");
    if (risk_code == 10) {
        printf("LOAN APPROVED\n");
        fprintf(f, "LOAN APPROVED\n");
    } else if (risk_code == 20) {
        printf("MANUAL REVIEW REQUIRED\n");
        fprintf(f, "MANUAL REVIEW REQUIRED\n");
    } else {
        printf("LOAN REJECTED\n");
        fprintf(f, "LOAN REJECTED\n");
    }
    
    printf("=======================================================\n");

    fclose(f);

    // Call the pure C PDF compiler
    writePdf(dest_pdf_path, cv_txt_path, report_path);

    // Cleanup temp cache
    remove(report_path);
}
