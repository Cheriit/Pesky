//
// Created by cherit on 12/30/20.
//

#include "messaging.h"

void getContent(char *message, char *content) {
    sscanf(message, "type:%*[^;];content:%s;", content);
}

void getType(char *message, char *type) {
    sscanf(message, "type:%[^;];content:%*s;", type);
}

void sendMessage(User *user, char message[], int size) {
    if(user != NULL)
    {
        pthread_mutex_lock(&user->semaphore);
        write(user->connection_descriptor, message, size);
        pthread_mutex_unlock(&user->semaphore);
    }
}

void sendConfirmMessage(User* user, char content[])
{
    char message[MAX_CONFIRM_SIZE] = "type:confirm;content:";
    strcat(message, content);
    strcat(message, ";\n");
    sendMessage(user, message, MAX_CONFIRM_SIZE);
}

void sendErrorMessage(User* user, char content[])
{
    char message[MAX_ERROR_SIZE] = "type:error;content:";
    strcat(message, content);
    strcat(message, ";\n");
    sendMessage(user, message, MAX_ERROR_SIZE);
}

void sendJoinedMessage(User* receiver_user, User* caller_user)
{
    char message[MAX_ERROR_SIZE] = "type:joined;content:";
    strcat(message, caller_user->username);
    strcat(message, ";\n");
    sendMessage(receiver_user, message, MAX_ERROR_SIZE);
}

void sendDisconnectMessage(User* receiver_user, User* caller_user)
{
    char message[MAX_ERROR_SIZE] = "type:disconnect;content:";
    strcat(message, caller_user->username);
    strcat(message, ";\n");
    sendMessage(receiver_user, message, MAX_ERROR_SIZE);
}

void forwardMessage(User* user, char message[])
{
    if (user != NULL)
    {
        pthread_mutex_lock(&user->semaphore);
        if (user->calls_to != NULL)
        {
            pthread_mutex_lock(&user->calls_to->semaphore);
            write(user->calls_to->connection_descriptor, message, strlen(message)*sizeof(char));
            pthread_mutex_unlock(&user->calls_to->semaphore);
        }
        pthread_mutex_unlock(&user->semaphore);
    }
}

int processMessage(struct thread_data_t *thread_data, char message[])
{
    char type[MAX_TYPE_SIZE] = "";
    char content[MAX_CONTENT_SIZE] = "";

    getType(message, type);
    if(strcmp(type, "call_to") == 0)
    {
    }
    else if(strcmp(type, "set_username") == 0)
    {
        getContent(message, content);
        pthread_mutex_lock(&thread_data->list->semaphore);
        User* user = find_on_usr_list(thread_data->list->next, content);
        if (user == NULL)
        {
            set_username(thread_data->user, content);
            printf("Changed username to: %s\n", content);
            sendConfirmMessage(thread_data->user, "Successfully changed username");
        }
        else
        {
            sendErrorMessage(thread_data->user, "User with this username already exists");
        }
        pthread_mutex_unlock(&thread_data->list->semaphore);
    }
    else if(strcmp(type, "audio") == 0 || strcmp(type, "video") == 0)
    {
        forwardMessage(thread_data->user, message);
    }
    else if(strcmp(type, "disconnect") == 0)
    {
        return 0;
    }
    else
    {
        if (DEBUG == 1)
        {
            getContent(message, content);
            printf("Recieved incorrect message type> %s %s\n", type, content);
        }
        sendErrorMessage(thread_data->user, "Recieved incorrect message");
    }
    return 1;
}
