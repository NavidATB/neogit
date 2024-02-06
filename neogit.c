

#define __USE_MISC // to enable some dirent.h features
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_DURATION 600

#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000
#define debug(x) printf("%s", x);

// global user:
bool todo_check(char *filepath);
bool eof_blank_space(char *filepath);
bool format_check(char *filepath);
bool balance_braces(char *filepath);
bool static_error_check(char *filepath);
bool file_size_check(char *filepath);
bool character_limit(char *filepath);
bool time_limit(char *filepath);
int add_to_staging_deleted(char *filepath, char *type);
bool compareFilePermissions(const char *filePath1, const char *filePath2);
void print_command(int argc, char *const argv[]);
int is_shortcut(int argc, char *const argv[]);
int run_init(int argc, char *const argv[]);
int create_configs();
bool availabe_checkout(char *commit_ID);

int run_add(int argc, char *const argv[]);
int add_to_staging(char *filepath, char *type);

int run_reset(int argc, char *const argv[]);
int remove_from_staging(char *filepath);

int run_commit(int argc, char *const argv[]);
int inc_last_commit_ID();
bool check_file_directory_exists(char *filepath);
int commit_staged_file(int commit_ID, char *filepath);
int track_file(char *filepath);
bool is_tracked(char *filepath);
int create_commit_file(int commit_ID, char *message, char *branch, int num, char *author);
int find_file_last_commit(char *filepath);

int run_checkout_commit(int argc, char *const argv[]);
int find_file_last_change_before_commit(char *filepath, int commit_ID);
int checkout_file(char *filepath, int commit_ID);

void print_command(int argc, char *const argv[])
{
  for (int i = 0; i < argc; i++)
  {
    fprintf(stdout, "%s ", argv[i]);
  }
  fprintf(stdout, "\n");
}

int run_init(int argc, char *const argv[])
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return 1;
  // printf("Current working dir: %s\n", cwd);
  char tmp_cwd[1024];
  bool exists = false;
  struct dirent *entry;
  do
  {
    // find .neogit
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
        exists = true;
    }
    closedir(dir);

    // update current working directory
    if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
      return 1;

    // change cwd to parent
    if (strcmp(tmp_cwd, "/") != 0)
    {
      if (chdir("..") != 0)
        return 1;
    }

  } while (strcmp(tmp_cwd, "/") != 0);

  // return to the initial cwd
  if (chdir(cwd) != 0)
    return 1;

  // printf("Current working dir: %s\n", tmp_cwd);
  if (!exists)
  {
    if (mkdir(".neogit", 0755) != 0)
      return 1;
    return create_configs();
  }
  else
  {
    perror("neogit repository has already initialized");
  }
  return 0;
}

bool exists_neogit()
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return 1;
  char tmp_cwd[1024];
  bool exists = false;
  struct dirent *entry;
  do
  {
    // find .neogit
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
        exists = true;
    }
    closedir(dir);

    // update current working directory
    if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
      return 1;

    // change cwd to parent
    if (strcmp(tmp_cwd, "/") != 0)
    {
      if (chdir("..") != 0)
        return 1;
    }

  } while (strcmp(tmp_cwd, "/") != 0);
  if (chdir(cwd) != 0)
    return 1;

  if (exists == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void hooks()
{
  FILE *file = fopen(".neogit/hooks", "w");
  fprintf(file, "todo-check\n");
  fprintf(file, "eof-blank-space\n");
  fprintf(file, "format-check\n");
  fprintf(file, "balance-braces\n");
  fprintf(file, "indentation-check\n");
  fprintf(file, "static-error-check\n");
  fprintf(file, "file-size-check\n");
  fprintf(file, "character-limit\n");
  fprintf(file, "time-limit\n");
  fclose(file);
}

int create_configs()
{
  char current_directory[1024];
  char email[100];
  char username[100];
  getcwd(current_directory, sizeof(current_directory));
  strcat(current_directory, "/user");
  FILE *readfile = fopen(current_directory, "r");
  FILE *readfile1 = fopen("~/.neogitconfig", "r");
  if (readfile != NULL && readfile1 == NULL)
  {

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), readfile) != NULL)
    {
      char *point = strstr(line, "username");
      if (point != NULL)
      {
        sscanf(line, "username: %s", username);
        continue;
      }
      char *point1 = strstr(line, "useremail");
      if (point1 != NULL)
      {
        sscanf(line, "useremail: %s", email);
        continue;
      }
    }
  }
  else if (readfile == NULL && readfile1 != NULL)
  {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), readfile1) != NULL)
    {
      char *point = strstr(line, "username");
      if (point != NULL)
      {
        sscanf(line, "username: %s", username);
        continue;
      }
      char *point1 = strstr(line, "useremail");
      if (point1 != NULL)
      {
        sscanf(line, "useremail: %s", email);
        continue;
      }
    }
  }

  FILE *file = fopen(".neogit/config", "w");
  if (file == NULL)
    return 1;

  fprintf(file, "username: %s\n", username);
  fprintf(file, "email: %s\n", email);
  fprintf(file, "last_commit_ID: %d\n", 0);
  fprintf(file, "current_commit_ID: %d\n", 0);
  fprintf(file, "branch: %s", "master");

  fclose(file);

  // create commits folder
  if (mkdir(".neogit/commits", 0755) != 0)
    return 1;

  // create files folder
  if (mkdir(".neogit/files", 0755) != 0)
    return 1;

  if (mkdir(".neogit/staged", 0755) != 0)
    return 1;

  if (mkdir(".neogit/branches", 0755) != 0)
    return 1;
  if (mkdir(".neogit/branches/master", 0755) != 0)
    return 1;
  if (mkdir(".neogit/tags", 0755) != 0)
    return 1;
  hooks();
  file = fopen(".neogit/staging", "w");
  fclose(file);

  file = fopen(".neogit/tracks", "w");
  fclose(file);

  return 0;
}

int run_add(int argc, char *const argv[])
{
  if (argc < 3)
  {
    perror("please specify a file");
    return 1;
  }

  for (int i = 2; i < argc; i++)
  {
    if (strcmp(argv[i], "-f") == 0)
    {

      continue;
    }

    // TODO: handle command in non-root directories
    struct stat path_stat;
    stat(argv[i], &path_stat);
    if (S_ISREG(path_stat.st_mode))
    {

      char *type = "file";
      if (fopen(argv[i], "r") == NULL)
      {
        perror("file not found!");
        return 1;
      }
      add_to_staging(argv[i], type);
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
      DIR *dir = opendir(argv[i]);
      struct dirent *entry;
      if (dir == NULL)
      {
        perror("directory not found!");
        return 1;
      }
      while ((entry = readdir(dir)) != NULL)
      {
        if (entry->d_type == DT_REG)
        {
          char path[MAX_FILENAME_LENGTH];
          strcpy(path, argv[i]);
          strcat(path, "/");
          strcat(path, entry->d_name);

          char *type = "dir";
          add_to_staging(path, type);
        }
      }
      closedir(dir);
    }
    else
    {
      if (is_tracked(argv[i]))
      {
        add_to_staging_deleted(argv[i], "file");
      }
      else
      {
        perror("file or directory not found!");
        return 1;
      }
    }
  }
  return 0;
}
int add_to_staging_deleted(char *filepath, char *type)
{
  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;
  FILE *tmpfile = fopen(".neogit/tmp_staging", "w");
  char line[MAX_LINE_LENGTH];
  int flag = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, filepath) != NULL)
    {
      char newline[MAX_LINE_LENGTH];
      strtok(line, " ");
      strcpy(newline, line);
      strcat(newline, " staged deleted\n");
      fputs(newline, tmpfile);
      flag = 1;
    }
    else
    {
      fputs(line, tmpfile);
    }
  }

  if (flag == 0)
  {
    fprintf(tmpfile, "%s deleted\n", filepath);
  }
  fclose(file);
  fclose(tmpfile);
  remove(".neogit/staging");
  rename(".neogit/tmp_staging", ".neogit/staging");

  return 0;
}

int is_samed_file(char *filepath1, char *filepath2)
{

  FILE *file1 = fopen(filepath1, "r");
  if (file1 == NULL)
  {

    return 1;
  }
  FILE *file2 = fopen(filepath2, "r");
  if (file2 == NULL)
  {

    return 1;
  }

  char line1[MAX_LINE_LENGTH];
  char line2[MAX_LINE_LENGTH];
  while (fgets(line1, sizeof(line1), file1) != NULL && fgets(line2, sizeof(line2), file2) != NULL)
  {

    if (strcmp(line1, line2) != 0)

      return 0;
  }
  fclose(file1);
  fclose(file2);
  return 1;
}

typedef struct OutputNode
{
  char *name;
  char *status;
  struct OutputNode *child; // شاخه‌ها
  struct OutputNode *next;  // خروجی‌های هم‌سطح
} OutputNode;

// تابع برای افزودن یک خروجی به شاخه
void addChild(OutputNode *parent, const char *name, const char *status)
{
  OutputNode *child = (OutputNode *)malloc(sizeof(OutputNode));
  child->name = strdup(name);
  child->status = strdup(status);
  child->child = NULL;
  child->next = parent->child;
  parent->child = child;
}
void printHorizontalLines(int count)
{
  for (int i = 0; i < count; ++i)
  {
    printf("    ");
  }
}

void printOutput(OutputNode *node, int depth, int isLast)
{
  if (depth > 0)
  {
    printHorizontalLines(depth - 1);

    printf(isLast ? "`-- " : "|-- ");
  }

  printf("%s : %s\n", node->name, node->status);

  OutputNode *child = node->child;
  while (child != NULL)
  {
    int isLastChild = (child->next == NULL);
    printOutput(child, depth + 1, isLastChild);
    child = child->next;
  }
}

// تابع برای آزاد کردن حافظه مرتبط با ساختار
void freeOutput(OutputNode *node)
{
  free(node->name);
  free(node->status);

  // آزاد کردن حافظه فرزندان به صورت بازگشتی
  OutputNode *child = node->child;
  while (child != NULL)
  {
    OutputNode *temp = child;
    child = child->next;
    freeOutput(temp);
  }

  free(node);
}

int add_depth(int argc, char *const argv[], int depth, const char *path, OutputNode *outputNode)
{
  DIR *dir = opendir(path);
  struct dirent *entry;

  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }

  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      char filepath[MAX_FILENAME_LENGTH];
      snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

      DIR *dir1 = opendir(".neogit/staged");
      if (dir1 == NULL)
      {
        perror("Error opening staged directory");
        closedir(dir);
        return 1;
      }

      struct dirent *entry1;
      int flag = 0;

      while ((entry1 = readdir(dir1)) != NULL)
      {
        if (entry1->d_type == DT_REG)
        {
          char filepath1[MAX_FILENAME_LENGTH];
          snprintf(filepath1, sizeof(filepath1), ".neogit/staged/%s", entry1->d_name);

          if (is_samed_file(filepath, filepath1))
          {
            addChild(outputNode, entry->d_name, "staged");
            flag = 1;
            break;
          }
        }
      }

      if (flag == 0)
      {
        addChild(outputNode, entry->d_name, "not staged");
      }

      closedir(dir1);
    }
    else if (entry->d_type == DT_DIR && depth > 1)
    {
      if (strcmp(entry->d_name, ".neogit") == 0)
      {
        continue;
      }
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
      {
        // ... کد مربوط به پوشه‌ها

        char subdir_path[MAX_FILENAME_LENGTH];
        snprintf(subdir_path, sizeof(subdir_path), "%s/%s", path, entry->d_name);
        OutputNode *childNode = (OutputNode *)malloc(sizeof(OutputNode));
        childNode->name = strdup(entry->d_name);
        childNode->status = strdup("directory");
        childNode->child = NULL;
        childNode->next = outputNode->child;
        outputNode->child = childNode;

        add_depth(argc, argv, depth - 1, subdir_path, childNode);
      }
    }
  }

  closedir(dir);
  return 0;
}

/*int main(int argc, char *const argv[]) {
    // در اینجا باید مسیر مورد نظر خود را قرار دهید
    const char *start_path = ".";

    OutputNode *rootNode = (OutputNode *)malloc(sizeof(OutputNode));
    rootNode->name = strdup("Root");
    rootNode->status = strdup("");
    rootNode->child = NULL;
    rootNode->next = NULL;

    add_depth(argc, argv, 2, start_path, rootNode);

    // چاپ خروجی‌ها به صورت شاخه‌ای
    printOutput(rootNode, 0);

    // آزاد کردن حافظه
    freeOutput(rootNode);

    return 0;
}*/

/*int add_depth(int argc , char *const argv[],int depth,char *path){


  DIR *dir = opendir(path);
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int flag = 0;
  while ((entry = readdir(dir)) != NULL)
  {
    DIR *dir1 = opendir(".neogit/staged");
    struct dirent *entry1;
    entry1 = readdir(dir1);
    if (entry->d_type == DT_REG)
    {
      char filepath[MAX_FILENAME_LENGTH];
      strcpy(filepath, entry->d_name);
      //printf("%s\n", entry1->d_name);
      while ((entry1 = readdir(dir1)) != NULL)
      {

        if (entry1->d_type == DT_REG)
        {
          char filepath1[MAX_FILENAME_LENGTH];
          strcpy(filepath1, ".neogit/staged/");

          strcat(filepath1, entry1->d_name);
          //printf("%s\n", filepath1);
          if (is_samed_file(filepath, filepath1) == 1)
          {
            printf("%s : staged\n", filepath);
            flag =1;
            break;
          }
        }
      }
      if(flag==0){
        printf("%s : not staged\n", filepath);
      }
    }
    else if (entry->d_type == DT_DIR && depth > 1)
    {
      printf("fsd");
      add_depth(argc , argv , depth-1 , entry->d_name);

    }
  }
  return 0;

}*/
int add_to_staging(char *filepath, char *type)
{
  DIR *dir = opendir(".neogit/staged");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {

    if (entry->d_type == DT_REG)
    {
      char filepath1[MAX_FILENAME_LENGTH];
      strcpy(filepath1, ".neogit/staged/");
      strcat(filepath1, entry->d_name);

      int s = is_samed_file(filepath, filepath1);

      if (s == 1)
      {
        printf("%s : this file is already staged\n", filepath);
        return 0;
      }
    }
  }

  FILE *read_file, *write_file;

  char read_path[MAX_FILENAME_LENGTH];
  strcpy(read_path, filepath);
  char write_path[MAX_FILENAME_LENGTH];
  if (strcmp(type, "file") == 0)
  {
    if (strstr(filepath, ".neogit") != NULL)
    {
      perror("you can't add .neogit files");
      return 1;
    }
    else if (strstr(filepath, "/"))
    {
      char *filename = strrchr(filepath, '/');
      // printf("%s\n", filename);
      strcpy(write_path, ".neogit/staged");
      strcat(write_path, filename);
      // printf("%s\n", write_path);
    }
    else
    {
      strcpy(write_path, ".neogit/staged/");
      strcat(write_path, filepath);
    }
  }
  else if (strcmp(type, "dir") == 0)
  {

    char *filename = strrchr(filepath, '/');
    strcpy(write_path, ".neogit/staged");
    strcat(write_path, filename);
  }

  read_file = fopen(read_path, "r");
  if (read_file == NULL)
    return 1;

  write_file = fopen(write_path, "w");
  if (write_file == NULL)
    return 1;
  char buffer;
  buffer = fgetc(read_file);
  while (buffer != EOF)
  {
    fputc(buffer, write_file);
    buffer = fgetc(read_file);
  }
  fclose(read_file);
  fclose(write_file);

  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    int length = strlen(line);

    // remove '\n'
    if (length > 0 && line[length - 1] == '\n')
    {
      line[length - 1] = '\0';
    }

    if (strcmp(filepath, line) == 0)
      return 0;
  }
  fclose(file);

  file = fopen(".neogit/staging", "r");
  FILE *tmpfile = fopen(".neogit/tmp_staging", "w");
  if (file == NULL)
    return 1;

  int flag = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, filepath) != NULL && strstr(line, "unstaged") != NULL)
    {
      char newline[MAX_LINE_LENGTH];
      strtok(line, " ");
      strcpy(newline, line);
      strcat(newline, " staged\n");
      fputs(newline, tmpfile);
      flag = 1;
    }
    else
    {
      fputs(line, tmpfile);
    }
  }
  if (flag == 0)
  {
    fprintf(tmpfile, "%s staged\n", filepath);
  }

  fclose(file);
  fclose(tmpfile);
  remove(".neogit/staging");
  rename(".neogit/tmp_staging", ".neogit/staging");

  return 0;
}

int run_reset(int argc, char *const argv[])
{
  // TODO: handle command in non-root directories
  if (argc < 3)
  {
    perror("please specify a file");
    return 1;
  }
  for (int i = 2; i < argc; i++)
  {
    if (strcmp(argv[i], "-f") == 0)
    {
      continue;
    }
    struct stat path_stat;
    stat(argv[i], &path_stat);
    if (S_ISREG(path_stat.st_mode))
    {
      char *type = "file";
      if (fopen(argv[i], "r") == NULL)
      {
        perror("file not found!");
        return 1;
      }
      // printf("%s\n", argv[i]);
      remove_from_staging(argv[i]);
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
      DIR *dir = opendir(argv[i]);
      struct dirent *entry;
      if (dir == NULL)
      {
        perror("directory not found!");
        return 1;
      }
      while ((entry = readdir(dir)) != NULL)
      {
        if (entry->d_type == DT_REG)
        {
          char path[MAX_FILENAME_LENGTH];
          strcpy(path, argv[i]);
          strcat(path, "/");
          strcat(path, entry->d_name);

          char *type = "dir";
          remove_from_staging(path);
        }
        else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
          char path[MAX_FILENAME_LENGTH];
          strcpy(path, argv[i]);
          strcat(path, "/");
          strcat(path, entry->d_name);

          run_reset(argc, path);
        }
      }
      closedir(dir);
    }
    else
    {
      perror("file or directory not found!");
      return 1;
    }
  }
  return 0;
}
int reset_undo(int argc, char *const argv[])
{
  if (argc < 3)
  {
    perror("please specify a file");
    return 1;
  }
  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, "stage") != NULL)
    {
      strtok(line, " ");
      remove_from_staging(line);
    }
  }
  return 0;
}
int remove_from_staging(char *filepath)
{
  char filepath1[MAX_FILENAME_LENGTH];
  DIR *dir = opendir(".neogit/staged");
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  struct dirent *entry;
  const char *lastSlash = strrchr(filepath, '/');

  const char *fileName = (lastSlash) ? (lastSlash + 1) : filepath;

  // printf("File Name: %s\n", fileName);
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {

      // printf("ppp%s\n", name);

      if (strcmp(entry->d_name, fileName) == 0)
      {

        strcpy(filepath1, ".neogit/staged/");
        strcat(filepath1, fileName);
        // printf("%s\n", filepath1);
        if (remove(filepath1) != 0)
          return 1;
      }
    }
  }

  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;

  FILE *tmp_file = fopen(".neogit/tmp_staging", "w");
  if (tmp_file == NULL)
    return 1;

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    int length = strlen(line);

    // remove '\n'
    if (length > 0 && line[length - 1] == '\n')
    {
      line[length - 1] = '\0';
    }

    if (strstr(line, filepath) == NULL)
      fputs(line, tmp_file);
    else if (strstr(line, filepath) != NULL && strstr(line, "staged") != NULL)

    {
      // printf("%s\n", line);
      char newline[MAX_LINE_LENGTH];
      strtok(line, " ");

      strcpy(newline, line);
      strcat(newline, " unstaged\n");

      fputs(newline, tmp_file);
    }
  }
  fclose(file);
  fclose(tmp_file);

  remove(".neogit/staging");
  rename(".neogit/tmp_staging", ".neogit/staging");
}

bool availabe_commit()
{
  FILE *file = fopen(".neogit/config", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  int last_commit_ID, current_commit_ID;
  while (fgets(line, sizeof(line), file) != NULL)
  {

    if (strncmp(line, "last_commit_ID", 14) == 0)
    {
      sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
    }
    if (strncmp(line, "current_commit_ID", 17) == 0)
    {
      sscanf(line, "current_commit_ID: %d\n", &current_commit_ID);
    }
  }
  fclose(file);
  if (last_commit_ID == current_commit_ID)
  {
    return true;
  }
  return false;
}
int run_commit(int argc, char *const argv[])
{
  if (argc < 4)
  {
    perror("please use the correct format");
    return 1;
  }
  if (availabe_commit() == false)
  {
    perror("you should go back to the HEAD before commit");
    return 1;
  }
  char message[MAX_MESSAGE_LENGTH];
  if (argv[2][1] == 's')
  {
    if (is_shortcut(argc, argv) == 1)
    {

      FILE *file = fopen(".neogit/message", "r");
      if (file == NULL)
        return 1;
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        if (strstr(line, argv[3]) != NULL)
        {
          strtok(line, " ");
          strcpy(message, line);
          break;
        }
      }
    }
    else
    {
      perror("please use the correct format");
      return 1;
    }
  }
  else if (argv[2][1] == 'm')
  {
    strcpy(message, argv[3]);
  }
  else
  {
    perror("please use the correct format");
    return 1;
  }

  int commit_ID = inc_last_commit_ID();
  char branch[MAX_LINE_LENGTH];
  char author[MAX_LINE_LENGTH];
  char username[MAX_LINE_LENGTH];
  char email[MAX_LINE_LENGTH];

  FILE *file = fopen(".neogit/config", "r");
  if (file == NULL)
    return 1;

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strncmp(line, "username", 8) == 0)
    {
      sscanf(line, "username: %s", username);
    }
    else if (strncmp(line, "email", 5) == 0)
    {
      sscanf(line, "email: %s", email);
    }
    else if (strncmp(line, "branch", 6) == 0)
    {
      sscanf(line, "branch: %s", branch);
    }
  }
  strcpy(author, username);
  strcat(author, " <");
  strcat(author, email);
  strcat(author, ">");

  fclose(file);

  if (commit_ID == -1)
    return 1;
  DIR *dir = opendir(".neogit/staged");
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int number_of_files_commited = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {

    if (entry->d_type == DT_REG)
    {
      FILE *file = fopen(".neogit/staging", "r");
      char path[MAX_FILENAME_LENGTH];
      if (file == NULL)
        return 1;
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        if (strstr(line, entry->d_name) != NULL)
        {
          strtok(line, " ");
          strcpy(path, line);
          break;
        }
      }
      // printf("%s\n", entry->d_name);
      char filepath[MAX_FILENAME_LENGTH];
      char filepath1[MAX_FILENAME_LENGTH];

      strcpy(filepath, ".neogit/staged/");
      strcat(filepath, entry->d_name);

      strcpy(filepath1, ".neogit/files/");
      strcat(filepath1, entry->d_name);
      if (!check_file_directory_exists(entry->d_name))
      {
        if (mkdir(filepath1, 0755) != 0)
        {
          // printf("1ok\n");
          return 1;
        }
      }
      if (commit_staged_file(commit_ID, filepath) != 0)
        return 1;
      if (track_file(path) != 0)
        return 1;
      number_of_files_commited++;
      remove(filepath);
    }
  }
  closedir(dir);
  if (number_of_files_commited == 0)
  {
    perror("there is no file to commit");
    return 1;
  }

  /*
    FILE *file = fopen(".neogit/staging", "r");
    if (file == NULL)
      return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
      int length = strlen(line);

      // remove '\n'
      if (length > 0 && line[length - 1] == '\n')
      {
        line[length - 1] = '\0';
      }

      if (!check_file_directory_exists(line))
      {
        char dir_path[MAX_FILENAME_LENGTH];
        strcpy(dir_path, ".neogit/files/");
        strcat(dir_path, line);
        if (mkdir(dir_path, 0755) != 0)
          return 1;
      }
      printf("commit %s\n", line);
      commit_staged_file(commit_ID, line);
      track_file(line);
    }
    fclose(file);
  */
  // free staging
  FILE *file1 = fopen(".neogit/staging", "w");
  if (file1 == NULL)
    return 1;
  fclose(file1);
  time_t rawTime;
  struct tm *timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);

  create_commit_file(commit_ID, message, branch, number_of_files_commited, author);
  fprintf(stdout, "commit successfully with commit ID %d ,at: %s", commit_ID, asctime(timeInfo));

  return 0;
}

// returns new commit_ID
int inc_last_commit_ID()
{
  FILE *file = fopen(".neogit/config", "r");
  if (file == NULL)
    return -1;

  FILE *tmp_file = fopen(".neogit/tmp_config", "w");
  if (tmp_file == NULL)
    return -1;

  int last_commit_ID;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strncmp(line, "last_commit_ID", 14) == 0)
    {
      sscanf(line, "last_commit_ID: %d\n", &last_commit_ID);
      last_commit_ID++;
      fprintf(tmp_file, "last_commit_ID: %d\n", last_commit_ID);
    }
    else if (strncmp(line, "current_commit_ID", 17) == 0)
    {
      fprintf(tmp_file, "current_commit_ID: %d\n", last_commit_ID);
    }
    else
    {
      fprintf(tmp_file, "%s", line);
    }
  }
  fclose(file);
  fclose(tmp_file);

  remove(".neogit/config");
  rename(".neogit/tmp_config", ".neogit/config");
  return last_commit_ID;
}

bool check_file_directory_exists(char *filepath)
{
  DIR *dir = opendir(".neogit/files");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_DIR && strcmp(entry->d_name, filepath) == 0)
      return true;
  }
  closedir(dir);

  return false;
}

int commit_staged_file(int commit_ID, char *filepath)
{
  // printf("%s\n", filepath);
  FILE *read_file, *write_file;
  char read_path[MAX_FILENAME_LENGTH];
  strcpy(read_path, filepath);
  const char *lastSlash = strrchr(filepath, '/');

  const char *fileName = (lastSlash) ? (lastSlash + 1) : filepath;
  char write_path[MAX_FILENAME_LENGTH];
  strcpy(write_path, ".neogit/files/");
  strcat(write_path, fileName);
  strcat(write_path, "/");
  char tmp[10];
  sprintf(tmp, "%d", commit_ID);
  strcat(write_path, tmp);
  // printf("%s\n", read_path);
  read_file = fopen(read_path, "r");
  if (read_file == NULL)
    return 1;
  // printf("%s\n", write_path);
  write_file = fopen(write_path, "w");
  if (write_file == NULL)
    return 1;

  char buffer;
  // printf("%c", buffer);
  buffer = fgetc(read_file);
  while (buffer != EOF)
  {
    fputc(buffer, write_file);
    buffer = fgetc(read_file);
  }

  fclose(read_file);
  fclose(write_file);

  return 0;
}

int track_file(char *filepath)
{

  if (is_tracked(filepath))
  {

    return 0;
  }
  FILE *file = fopen(".neogit/tracks", "a");

  if (file == NULL)
  {
    printf("error\n");
    return 1;
  }

  fprintf(file, "%s\n", filepath);
  fclose(file);

  return 0;
}

bool is_tracked(char *filepath)
{
  FILE *file = fopen(".neogit/tracks", "r");
  if (file == NULL)
    return false;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    int length = strlen(line);

    // remove '\n'
    if (length > 0 && line[length - 1] == '\n')
    {
      line[length - 1] = '\0';
    }

    if (strcmp(line, filepath) == 0)
      return true;
  }
  fclose(file);

  return false;
}

int create_commit_file(int commit_ID, char *message, char *branch, int num, char *author)
{
  char commit_filepath[MAX_FILENAME_LENGTH];
  strcpy(commit_filepath, ".neogit/commits/");
  char tmp[10];
  sprintf(tmp, "%d", commit_ID);
  strcat(commit_filepath, tmp);

  FILE *file = fopen(commit_filepath, "w");
  if (file == NULL)
    return 1;
  time_t rawTime;
  struct tm *timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);

  char timeString[10]; // برای نگهداری زمان به صورت جدا (ساعت:دقیقه:ثانیه)
  strftime(timeString, sizeof(timeString), "%H:%M:%S", timeInfo);

  char dateString[11]; // برای نگهداری تاریخ به صورت عدد
  strftime(dateString, sizeof(dateString), "%Y-%m-%d", timeInfo);

  // افزودن فرمت‌بندی به پرینت‌ف
  fprintf(file, "message: %s\n", message);
  fprintf(file, "date: %s\n", dateString);
  fprintf(file, "time: %s\n", timeString);
  fprintf(file, "branch: %s\n", branch);
  fprintf(file, "number of files: %d\n", num);
  fprintf(file, "author: %s\n", author);
  fprintf(file, "files:\n");

  FILE *track_file = fopen(".neogit/tracks", "r");
  if (track_file == NULL)
    return 1;

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), track_file) != NULL)
  {

    int length = strlen(line);

    // remove '\n'
    if (length > 0 && line[length - 1] == '\n')
    {
      line[length - 1] = '\0';
    }
    int file_last_commit_ID = find_file_last_commit(line);
    fprintf(file, "%s %d\n", line, file_last_commit_ID);
  }
  char branch_file[MAX_FILENAME_LENGTH];
  strcpy(branch_file, ".neogit/branches/");
  strcat(branch_file, branch);
  strcat(branch_file, "/");
  strcat(branch_file, tmp);
  // printf("%s %s\n", commit_filepath, branch_file);
  fclose(track_file);
  fclose(file);
  copy_file(commit_filepath, branch_file);
  /*DIR *dir = opendir(".");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    printf("sda");
    if (entry->d_type == DT_REG && is_tracked(entry->d_name))
    {
      printf("com = %s\n", entry->d_name);
      int file_last_commit_ID = find_file_last_commit(entry->d_name);
      fprintf(file, "%s %d\n", entry->d_name, file_last_commit_ID);
    }
  }
  closedir(dir);
  fclose(file);
  */
  return 0;
}

int find_file_last_commit(char *filepath)
{
  // printf("sfadf\n");
  const char *lastSlash = strrchr(filepath, '/');
  const char *fileName = (lastSlash) ? (lastSlash + 1) : filepath;
  // printf("%s\n", fileName);
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/files/");
  strcat(filepath_dir, filepath);

  int max = -1;

  DIR *dir = opendir(filepath_dir);
  struct dirent *entry;
  if (dir == NULL)
  {
    // printf("oh");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    // printf("df");
    if (entry->d_type == DT_REG)
    {
      int tmp = atoi(entry->d_name);
      max = max > tmp ? max : tmp;
    }
  }
  closedir(dir);
  // printf("%s %d\n", fileName, max);
  return max;
}

int run_checkout_commit(int argc, char *const argv[])
{
  if (argc < 3)
    return 1;

  int commit_ID = atoi(argv[2]);
  if (commit_ID == 0)
    return 1;

  char tmp[10];
  sprintf(tmp, "%d", commit_ID);

  if (availabe_checkout(tmp) == false)
  {
    perror("you have uncommited changes!");
    return 1;
  }
  char filepath[MAX_FILENAME_LENGTH];
  strcpy(filepath, ".neogit/commits/");
  strcat(filepath, tmp);
  FILE *file = fopen(filepath, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, "files") != NULL)
    {
      break;
    }
  }
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, ".txt") != NULL)
    {
      char *filename = strtok(line, " ");
      int last_commit_ID = atoi(strtok(NULL, " "));
      if (last_commit_ID == 0)
      {
        perror("file not found in this commit!");
        return 1;
      }
      if (checkout_file(filename, last_commit_ID) != 0)
        return 1;
    }
  }
  fclose(file);
  FILE *file1 = fopen(".neogit/config", "r");
  if (file1 == NULL)
    return 1;
  FILE *tmp_file = fopen(".neogit/tmp_config", "w");
  if (tmp_file == NULL)
    return 1;
  while (fgets(line, sizeof(line), file1) != NULL)
  {
    if (strncmp(line, "current_commit_ID", 17) == 0)
    {
      fprintf(tmp_file, "current_commit_ID: %d\n", commit_ID);
    }
    else
      fprintf(tmp_file, "%s", line);
  }
  fclose(file1);
  fclose(tmp_file);
  remove(".neogit/config");
  rename(".neogit/tmp_config", ".neogit/config");

  // checkout_file(entry->d_name, find_file_last_change_before_commit(entry->d_name, commit_ID));

  return 0;
}

int find_file_last_change_before_commit(char *filepath, int commit_ID)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/files/");
  strcat(filepath_dir, filepath);

  int max = -1;

  DIR *dir = opendir(filepath_dir);
  struct dirent *entry;
  if (dir == NULL)
    return 1;

  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      int tmp = atoi(entry->d_name);
      if (tmp > max && tmp <= commit_ID)
      {
        max = tmp;
      }
    }
  }
  closedir(dir);

  return max;
}

int checkout_file(char *filepath, int commit_ID)
{
  char src_file[MAX_FILENAME_LENGTH];
  strcpy(src_file, ".neogit/files/");
  strcat(src_file, filepath);
  char tmp[10];
  sprintf(tmp, "/%d", commit_ID);
  strcat(src_file, tmp);

  FILE *read_file = fopen(src_file, "r");
  if (read_file == NULL)
    return 1;
  FILE *write_file = fopen(filepath, "w");
  if (write_file == NULL)
    return 1;

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), read_file) != NULL)
  {
    fprintf(write_file, "%s", line);
  }

  fclose(read_file);
  fclose(write_file);

  return 0;
}

/*char *searchfile(char *filename)
{
  DIR *dir = opendir(".");
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      char path[MAX_FILENAME_LENGTH];
      strcpy(path, entry->d_name);
      searchfile(path);
    }
    else if (entry->d_type == DT_REG && strcmp(entry->d_name, filename) == 0)
    {
      char path[MAX_FILENAME_LENGTH];
      strcpy(path, entry->d_name);
      return path;
    }
  }
  closedir(dir);
  return NULL;
}
*/
int run_config_user(int argc, char *const argv[])
{
  if (argc < 4)
  {
    perror("please use a correct format!");
    return 1;
  }
  char current_directory[1024];
  char tmp_directory[1024];

  getcwd(current_directory, sizeof(current_directory));
  strcpy(tmp_directory, current_directory);
  strcat(current_directory, "/user");

  if (strcmp(argv[2], "-global") == 0)
  {
    FILE *file = fopen("/neogitconfig", "a");
    if (strcmp(argv[3], "user.name") == 0)
    {

      fprintf(file, "username: %s\n", argv[4]);
      fclose(file);
      return 0;
    }
    else if (strcmp(argv[3], "user.email") == 0)
    {

      fprintf(file, "useremail: %s\n", argv[4]);

      fclose(file);
      return 0;
    }
  }
  else if (strcmp(argv[2], "user.name") == 0)
  {

    FILE *file = fopen(current_directory, "a");
    char *local_username = argv[3];
    fprintf(file, "username: %s\n", local_username);
    fclose(file);
    return 0;
  }
  else if (strcmp(argv[2], "user.email") == 0)
  {
    FILE *file = fopen(current_directory, "a");
    char *local_useremail = argv[3];
    fprintf(file, "useremail: %s\n", local_useremail);
    fclose(file);
    return 0;
  }
  else
  {
    perror("please use a correct format!");
    return 1;
  }
}

bool is_config()
{
  char current_directory[1024];
  getcwd(current_directory, sizeof(current_directory));
  strcat(current_directory, "/user");
  FILE *file = fopen(current_directory, "r");
  if (file == NULL)
  {
    return false;
  }
  else
  {
    while (fgets(current_directory, sizeof(current_directory), file) != NULL)
    {
      char *point = strstr(current_directory, "username");
      if (point != NULL)
      {
        return true;
      }
      char *point1 = strstr(current_directory, "useremail");
      if (point1 != NULL)
      {
        return true;
      }
    }
  }
}

int save_alias(int argc, char *argv[])
{
  if (argc < 4)
  {
    perror("please use a correct format!");
    return 1;
  }

  char *alias_name[2];
  alias_name[0] = strtok(argv[2], ".");
  alias_name[1] = strtok(NULL, ".");
  char command[MAX_LINE_LENGTH];

  strncpy(command, argv[3], MAX_LINE_LENGTH);
  FILE *file = fopen("/alias", "a");
  if (file == NULL)
  {
    perror("error opening file!");
    return 1;
  }
  fprintf(file, "%s %s\n", alias_name[1], command);
  fclose(file);

  return 0;
}

int is_alias(int argc, char *const argv[])
{
  // printf("asda");
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }

  if (argc > 2)
  {
    // printf("%s", argv[1]);
    return 1;
  }
  char alias[MAX_LINE_LENGTH];
  strcpy(alias, argv[1]);
  FILE *file = fopen("/alias", "r");
  if (file == NULL)
  {

    return 1;
  }
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    char *point = strstr(line, alias);
    char a[1] = "\n";
    char *point1 = strstr(line, a);

    if (point != NULL && line[point - line - 1] != ' ')
    {
      char command[MAX_LINE_LENGTH];
      sscanf(line, "%s %s", alias, command);
      system(command);
      return 0;
    }
  }
  fclose(file);
  return 1;
}

int run_add_redo(int argc, char *const argv[])
{
  if (argc < 3)
  {
    perror("please specify a file");
    return 1;
  }
  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, "unstaged") != NULL)
    {
      char *filepath = strtok(line, " ");
      add_to_staging(filepath, "file");
    }
  }
  fclose(file);
  return 0;
}

int run_set(int argc, char *const argv[])
{
  FILE *file = fopen(".neogit/message", "a");
  if (file == NULL)
    return 1;
  char shortcut_message[MAX_LINE_LENGTH];
  strcpy(shortcut_message, argv[3]);
  char shortcut_name[MAX_LINE_LENGTH];
  strcpy(shortcut_name, argv[5]);
  fprintf(file, "%s %s\n", shortcut_name, shortcut_message);
  fclose(file);
  return 0;
}

int run_replace(int argc, char *const argv[])
{
  if (argc < 4)
  {
    perror("please use a correct format!");
    return 1;
  }
  char *shortcut_name = argv[5];
  char *shortcut_message = argv[3];
  FILE *file = fopen(".neogit/message", "r");
  if (file == NULL)
    return 1;
  FILE *tmp_file = fopen(".neogit/tmp_message", "w");
  if (tmp_file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  int flag = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, shortcut_name) != NULL)
    {
      int flag = 1;
      fprintf(tmp_file, "%s %s\n", shortcut_name, shortcut_message);
    }
    else
    {
      fprintf(tmp_file, "%s", line);
    }
  }

  if (flag == 0)
  {
    perror("shortcut not found!");
    return 1;
  }

  fclose(file);
  fclose(tmp_file);
  remove(".neogit/message");
  rename(".neogit/tmp_message", ".neogit/message");
  return 0;
}

int run_remove(int argc, char *const argv[])
{
  if (argc < 3)
  {
    perror("please use a correct format!");
    return 1;
  }
  char *shortcut_name = argv[3];
  FILE *file = fopen(".neogit/message", "r");
  if (file == NULL)
    return 1;
  FILE *tmp_file = fopen(".neogit/tmp_message", "w");
  if (tmp_file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  int flag = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, shortcut_name) == NULL)
    {
      fprintf(tmp_file, "%s", line);
    }
    else
    {
      int flag = 1;
    }
  }

  if (flag == 0)
  {
    perror("shortcut not found!");
    return 1;
  }

  fclose(file);
  fclose(tmp_file);
  remove(".neogit/message");
  rename(".neogit/tmp_message", ".neogit/message");
  return 0;
}

int is_shortcut(int argc, char *const argv[])
{
  FILE *file = fopen(".neogit/message", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, argv[3]) != NULL)
    {
      char *shortcut_message = strtok(line, " ");
      return 1;
    }
  }
  fclose(file);
  return 0;
}

void printCommitInfo(const char *filePath)
{
  FILE *file = fopen(filePath, "r");
  if (file == NULL)
  {
    perror("Error opening file");
    return;
  }

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, "message") != NULL)
    {
      printf("    %s", line);
    }
    else if (strstr(line, "date") != NULL)
    {
      printf("    %s", line);
    }
    else if (strstr(line, "time") != NULL)
    {
      printf("    %s", line);
    }
    else if (strstr(line, "author") != NULL)
    {
      printf("    %s", line);
    }
    else if (strstr(line, "number") != NULL)
    {
      printf("    %s", line);
    }
    else if (strstr(line, "branch") != NULL)
    {
      printf("    %s", line);
    }
  }

  fclose(file);
}

// خواندن فایل‌ها از دایرکتوری به ترتیب زمان ایجاد
void readFilesInOrder(const char *directoryPath, int numberOfCommitsToShow)
{
  struct dirent **files;
  int fileCount = scandir(directoryPath, &files, NULL, alphasort);

  if (fileCount < 0)
  {
    perror("Error scanning directory");
    return;
  }

  // چاپ اطلاعات مرتب‌شده کامیت‌ها
  for (int i = fileCount - 1; i >= 0 && numberOfCommitsToShow > 0 && files[i]->d_type == DT_REG; i--)
  {
    char commitFilePath[MAX_FILENAME_LENGTH];
    snprintf(commitFilePath, sizeof(commitFilePath), "%s/%s", directoryPath, files[i]->d_name);

    printf("- Commit ID: %s\n", files[i]->d_name);
    printCommitInfo(commitFilePath);

    if (numberOfCommitsToShow != 1000)
    {
      numberOfCommitsToShow--;
    }

    free(files[i]);
  }

  free(files);
}

int run_log(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }

  int number = 1000;
  if (argc > 2)
  {
    if (argv[2][0] == '-' && argv[2][1] == 'n')
    {
      number = atoi(argv[3]);
    }
  }
  //  printf("%d", number);
  const char *directoryPath = ".neogit/commits";

  readFilesInOrder(directoryPath, number);

  /* DIR *dir = opendir(".neogit/commits");
   struct dirent *entry;
   if (dir == NULL)
   {
     perror("Error opening current directory");
     return 1;
   }
   while ((entry = readdir(dir)) != NULL  && number > 0)
   {
     if (entry->d_type == DT_REG)
     {
       char commit_filepath[MAX_FILENAME_LENGTH];
       strcpy(commit_filepath, ".neogit/commits/");
       strcat(commit_filepath, entry->d_name);
       FILE *file = fopen(commit_filepath, "r");
       if (file == NULL)
         return 1;
       char line[MAX_LINE_LENGTH];

       printf("-commit ID: %s\n", entry->d_name);
       while (fgets(line, sizeof(line), file) != NULL)
       {

         if (strstr(line, "message") != NULL)
         {
           printf("    %s", line);
         }
         else if (strstr(line, "date") != NULL)
         {
           printf("    %s", line);
         }
         else if (strstr(line,"author") != NULL)
         {
           printf("    %s", line);
         }
         else if(strstr(line,"number") != NULL)
         {
           printf("    %s", line);
         }
         else if(strstr(line,"branch") != NULL)
         {
           printf("    %s", line);
         }


       }
       fclose(file);
       if(number!=1000){
         number--;
       }
     }
   }*/
  return 0;
}

int run_log_branch(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }

  char branch[MAX_LINE_LENGTH];
  strcpy(branch, argv[3]);
  if (strcmp(branch, "master") != 0)
  {
    DIR *dir = opendir(".neogit/branches");
    struct dirent *entry;
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    int flag = 0;
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_DIR)
      {
        if (strcmp(entry->d_name, branch) == 0)
        {
          flag = 1;
          break;
        }
      }
    }
    if (flag == 0)
    {
      perror("branch not found!");
      return 1;
    }
  }
  const char *directoryPath = ".neogit/commits";
  struct dirent **files;
  int fileCount = scandir(directoryPath, &files, NULL, alphasort);

  if (fileCount < 0)
  {
    perror("Error scanning directory");
    return 1;
  }

  // چاپ اطلاعات مرتب‌شده کامیت‌ها
  for (int i = fileCount - 1; i >= 0 && files[i]->d_type == DT_REG; i--)
  {
    char commitFilePath[MAX_FILENAME_LENGTH];
    snprintf(commitFilePath, sizeof(commitFilePath), "%s/%s", directoryPath, files[i]->d_name);

    FILE *file = fopen(commitFilePath, "r");
    if (file == NULL)

      return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, "branch") != NULL)
      {
        if (strstr(line, branch) != NULL)
        {
          printf("- Commit ID: %s\n", files[i]->d_name);
          printCommitInfo(commitFilePath);
          break;
        }
      }
    }
    fclose(file);
    free(files[i]);
  }

  free(files);
  return 0;
}

int run_log_author(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  char author[MAX_LINE_LENGTH];
  strcpy(author, argv[3]);
  const char *directoryPath = ".neogit/commits";
  struct dirent **files;
  int fileCount = scandir(directoryPath, &files, NULL, alphasort);

  if (fileCount < 0)
  {
    perror("Error scanning directory");
    return 1;
  }

  // چاپ اطلاعات مرتب‌شده کامیت‌ها
  for (int i = fileCount - 1; i >= 0 && files[i]->d_type == DT_REG; i--)
  {
    char commitFilePath[MAX_FILENAME_LENGTH];
    snprintf(commitFilePath, sizeof(commitFilePath), "%s/%s", directoryPath, files[i]->d_name);

    printf("- Commit ID: %s\n", files[i]->d_name);
    FILE *file = fopen(commitFilePath, "r");
    if (file == NULL)
    {
      return 1;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, "author") != NULL)
      {
        if (strstr(line, author) != NULL)
        {
          printCommitInfo(commitFilePath);
          break;
        }
      }
    }
    fclose(file);
    free(files[i]);
  }

  free(files);
  return 0;
}

int run_log_date(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  char date[MAX_LINE_LENGTH];
  strcpy(date, argv[3]);
  const char *directoryPath = ".neogit/commits";
  struct dirent **files;
  int fileCount = scandir(directoryPath, &files, NULL, alphasort);

  if (fileCount < 0)
  {
    perror("Error scanning directory");
    return 1;
  }

  // چاپ اطلاعات مرتب‌شده کامیت‌ها
  for (int i = fileCount - 1; i >= 0 && files[i]->d_type == DT_REG; i--)
  {
    char commitFilePath[MAX_FILENAME_LENGTH];
    snprintf(commitFilePath, sizeof(commitFilePath), "%s/%s", directoryPath, files[i]->d_name);

    FILE *file = fopen(commitFilePath, "r");
    if (file == NULL)
    {
      return 1;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strcmp(argv[2], "-since") == 0)
      {

        if (strstr(line, "date") != NULL)
        {
          // printf("%s", line);
          char date1[11];
          sscanf(line, "date: %s", date1);
          // printf(" a = %s ", date1);
          if (date1 - date >= 0)
          {
            printf("- Commit ID: %s\n", files[i]->d_name);
            printCommitInfo(commitFilePath);
            break;
          }
        }
      }
      else if (strcmp(argv[2], "-befor") == 0)
      {
        if (strstr(line, "date") != NULL)
        {
          char date1[11];
          sscanf(line, "date: %s", date1);
          if (date1 - date <= 0)
          {
            printf("- Commit ID: %s\n", files[i]->d_name);
            printCommitInfo(commitFilePath);
            break;
          }
        }
      }
    }
    fclose(file);
    free(files[i]);
  }

  free(files);
  return 0;
}

int run_log_word(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }

  const char *directoryPath = ".neogit/commits";
  struct dirent **files;
  int fileCount = scandir(directoryPath, &files, NULL, alphasort);

  if (fileCount < 0)
  {
    perror("Error scanning directory");
    return 1;
  }

  // چاپ اطلاعات مرتب‌شده کامیت‌ها
  for (int i = fileCount - 1; i >= 0 && files[i]->d_type == DT_REG; i--)
  {
    char commitFilePath[MAX_FILENAME_LENGTH];
    snprintf(commitFilePath, sizeof(commitFilePath), "%s/%s", directoryPath, files[i]->d_name);

    FILE *file1 = fopen(commitFilePath, "r");
    if (file1 == NULL)
    {
      return 1;
    }

    for (int j = 3; j < argc; j++)
    {
      char word[MAX_LINE_LENGTH];
      strcpy(word, argv[j]);

      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file1) != NULL)
      {
        if (strstr(line, word) != NULL && strstr(line, "message") != NULL)
        {
          printf("- Commit ID: %s\n", files[i]->d_name);
          printCommitInfo(commitFilePath);
          break;
        }
      }
    }

    fclose(file1);
  }

  free(files);
  return 0;
}

bool is_staged(char *filepath)
{
  DIR *dir = opendir(".neogit/staged");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/staged/");
    strcat(filepath_dir, entry->d_name);
    if (entry->d_type == DT_REG && is_samed_file(filepath, filepath_dir))
      return true;
  }
  closedir(dir);

  return false;
}

int is_M(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/files/");
  strcat(filepath_dir, filepath);

  int last_commit_ID;
  DIR *dir2 = opendir(".neogit/commits");
  struct dirent *entry2;
  if (dir2 == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry2 = readdir(dir2)) != NULL)
  {
    if (entry2->d_type == DT_REG)
    {
      last_commit_ID = atoi(entry2->d_name);
    }
  }
  char filepath_dir1[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir1, ".neogit/commits/");
  char tmp[10];
  sprintf(tmp, "%d", last_commit_ID);
  strcat(filepath_dir1, tmp);
  FILE *file = fopen(filepath_dir1, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  int last_commit_ID_file;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, filepath) != NULL)
    {
      sscanf(line, "%s %d", filepath, &last_commit_ID_file);
      // printf("id = %d\n", last_commit_ID_file);
    }
  }
  char last_commit_ID_file1[MAX_FILENAME_LENGTH];
  sprintf(last_commit_ID_file1, "%d", last_commit_ID_file);

  closedir(dir2);
  fclose(file);

  strcat(filepath_dir, "/");
  strcat(filepath_dir, last_commit_ID_file1);
  // printf("%s %s\n", filepath, filepath_dir);
  if (is_samed_file(filepath, filepath_dir))
  {
    return 1;
  }
  else if (!is_samed_file(filepath, filepath_dir))
  {
    if (compareFilePermissions(filepath, filepath_dir) == false)
    {
      return 2;
    }
  }

  return 3;
}

void printPermissions(mode_t permissions)
{
  printf("File permissions: %o\n", permissions & 0777);
}
bool compareFilePermissions(const char *filePath1, const char *filePath2)
{
  struct stat stats1, stats2;

  if (stat(filePath1, &stats1) != 0)
  {
    perror("Error getting file information for file 1");
    return 1;
  }

  if (stat(filePath2, &stats2) != 0)
  {
    perror("Error getting file information for file 2");
    return 1;
  }

  // printf("da");
  if (stats1.st_mode == stats2.st_mode)
  {
    //  printPermissions(stats1.st_mode);
    // printPermissions(stats2.st_mode);
    return true;
  }
  else
  {
    // printf("da");
    return false;
  }
}

int run_status(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  DIR *dir = opendir(".");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG && is_staged(entry->d_name) && strstr(entry->d_name, ".txt") != NULL)
    {
      if (is_tracked(entry->d_name) == false)
      {
        printf("%s -> +", entry->d_name);
        printf("A\n");
      }
      else
      {
        int m = is_M(entry->d_name);
        // printf("%d\n", m);
        if (is_M(entry->d_name) == 3)
        {
          printf("%s -> +", entry->d_name);
          printf("M\n");
        }
        else if (is_M(entry->d_name) == 2)
        {
          printf("%s -> +", entry->d_name);
          printf("T\n");
        }
      }
    }
    else if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL)
    {

      if (is_M(entry->d_name) == 3)
      {
        printf("%s -> -", entry->d_name);
        printf("M\n");
      }
      else if (is_M(entry->d_name) == 2)
      {
        printf("%s -> -", entry->d_name);
        printf("T\n");
      }
    }
  }
  FILE *file = fopen(".neogit/staging", "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, "deleted") != NULL)
    {
      char *filepath = strtok(line, " ");
      printf("%s -> -", filepath);
      printf("D\n");
    }
    else if (strstr(line, "stage") != NULL && strstr(line, "deleted") != NULL)
    {
      char *filepath = strtok(line, " ");
      printf("%s -> +", filepath);
      printf("D\n");
    }
  }
  return 0;
}

int run_branch(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  if (argc == 3)
  {

    char branch[MAX_LINE_LENGTH];
    strcpy(branch, argv[2]);
    DIR *dir = opendir(".neogit/branches");
    struct dirent *entry;
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_DIR)
      {
        if (strcmp(entry->d_name, branch) == 0)
        {
          perror("branch already exists!");
          return 1;
        }
      }
    }
    closedir(dir);
    char branch_dir[MAX_FILENAME_LENGTH];
    strcpy(branch_dir, ".neogit/branches/");
    strcat(branch_dir, branch);
    mkdir(branch_dir, 0755);

    int last_commit_ID;
    FILE *config = fopen(".neogit/config", "r");
    if (config == NULL)
      return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), config) != NULL)
    {
      if (strstr(line, "last_commit_ID") != NULL)
      {
        sscanf(line, "last_commit_ID: %d", &last_commit_ID);
        break;
      }
    }
    fclose(config);
    char last_commit_ID1[MAX_FILENAME_LENGTH];
    sprintf(last_commit_ID1, "/%d", last_commit_ID);

    char branch_file[MAX_FILENAME_LENGTH];
    strcpy(branch_file, ".neogit/branches/");
    strcat(branch_file, branch);
    strcat(branch_file, last_commit_ID1);
    FILE *file = fopen(branch_file, "w");
    if (file == NULL)

      return 1;
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/commits/");
    char tmp[10];
    sprintf(tmp, "%d", last_commit_ID);
    strcat(filepath_dir, tmp);

    FILE *last_commit = fopen(filepath_dir, "r");
    if (last_commit == NULL)
      return 1;

    while (fgets(line, sizeof(line), last_commit) != NULL)
    {
      if (strstr(line, "branch") == NULL)
      {
        // printf("%s", line);
        fprintf(file, "%s", line);
      }
      else
      {
        char line1[MAX_LINE_LENGTH];
        strcpy(line1, "branch: ");
        strcat(line1, branch);
        fprintf(file, "%s\n", line1);
      }
    }
    fclose(last_commit);
    fclose(file);

    return 0;
  }
  else if (argc == 2)
  {
    DIR *dir = opendir(".neogit/branches");
    struct dirent *entry;
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    printf("Branches:\n");

    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_DIR)
      {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
          printf(" -%s\n", entry->d_name);
        }
      }
    }
    closedir(dir);
  }
  return 0;
}

bool availabe_checkout(char *commit_ID)
{
  char filepath[MAX_FILENAME_LENGTH];
  strcpy(filepath, ".neogit/commits/");
  strcat(filepath, commit_ID);
  FILE *file = fopen(filepath, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, ".txt") != NULL)
    {
      char filepath[MAX_FILENAME_LENGTH];
      int last_commit_ID_file;
      sscanf(line, "%s %d", filepath, &last_commit_ID_file);
      char filepath_dir[MAX_FILENAME_LENGTH];
      strcpy(filepath_dir, ".neogit/files/");
      strcat(filepath_dir, filepath);
      char tmp[10];
      sprintf(tmp, "%d", last_commit_ID_file);
      strcat(filepath_dir, tmp);
      if (is_samed_file(filepath, filepath_dir))
      {
        continue;
      }
      else
      {
        return false;
      }
    }
  }
  fclose(file);
  return true;
}

int run_checkout_branch(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  char branch[MAX_LINE_LENGTH];
  strcpy(branch, argv[2]);
  DIR *dir = opendir(".neogit/branches");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int flag = 0;
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_DIR)
    {
      if (strcmp(entry->d_name, branch) == 0)
      {
        flag = 1;
        break;
      }
    }
  }
  if (flag == 0)
  {
    perror("branch not found!");
    return 1;
  }
  closedir(dir);
  char dir_path[MAX_FILENAME_LENGTH];
  strcpy(dir_path, ".neogit/branches/");
  strcat(dir_path, branch);
  char filepath_dir[MAX_FILENAME_LENGTH];

  DIR *dir2 = opendir(dir_path);
  struct dirent *entry2;
  if (dir2 == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int last_commit_ID;
  while ((entry2 = readdir(dir2)) != NULL)
  {
    if (entry2->d_type == DT_REG)
    {
      last_commit_ID = atoi(entry2->d_name);
    }
  }
  closedir(dir2);
  char tmp[10];
  sprintf(tmp, "%d", last_commit_ID);
  if (availabe_checkout(tmp) == false)
  {
    perror("you have uncommited changes!");
    return 1;
  }
  char filepath_dir1[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir1, ".neogit/branches/");
  strcat(filepath_dir1, branch);
  strcat(filepath_dir1, "/");
  strcat(filepath_dir1, tmp);
  FILE *file = fopen(filepath_dir1, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, ".txt") != NULL)
    {
      char filepath[MAX_FILENAME_LENGTH];
      int last_commit_ID_file;
      sscanf(line, "%s %d", filepath, &last_commit_ID_file);
      char filepath_dir[MAX_FILENAME_LENGTH];
      strcpy(filepath_dir, ".neogit/files/");
      char tmp[10];
      sprintf(tmp, "%d", last_commit_ID_file);
      strcat(filepath_dir, tmp);
      if (is_samed_file(filepath, filepath_dir))
      {
        continue;
      }
      else
      {
        remove(filepath);
        copy_file(filepath_dir, filepath);
      }
    }
  }
  fclose(file);

  FILE *config = fopen(".neogit/config", "r");
  FILE *config1 = fopen(".neogit/config1", "w");

  if (config == NULL)
    return 1;
  if (config1 == NULL)
    return 1;
  while (fgets(line, sizeof(line), config) != NULL)
  {
    if (strstr(line, "branch") == NULL)
    {
      fprintf(config1, "%s", line);
    }
    else
    {
      fprintf(config1, "branch: %s\n", branch);
    }
  }
  fclose(config);
  fclose(config1);
  remove(".neogit/config");
  rename(".neogit/config1", ".neogit/config");

  return 0;
}
int run_checkout_HEAD()
{
  FILE *config = fopen(".neogit/config", "r");
  if (config == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  char branch[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), config) != NULL)
  {
    if (strstr(line, "branch") != NULL)
    {
      sscanf(line, "branch: %s", branch);
      break;
    }
  }
  fclose(config);
  char dir_path[MAX_FILENAME_LENGTH];
  strcpy(dir_path, ".neogit/branches/");
  strcat(dir_path, branch);
  char filepath_dir[MAX_FILENAME_LENGTH];

  DIR *dir2 = opendir(dir_path);
  struct dirent *entry2;
  if (dir2 == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int last_commit_ID;
  while ((entry2 = readdir(dir2)) != NULL)
  {
    if (entry2->d_type == DT_REG)
    {
      last_commit_ID = atoi(entry2->d_name);
    }
  }
  closedir(dir2);
  char tmp[10];
  sprintf(tmp, "%d", last_commit_ID);
  /* if (availabe_checkout(tmp) == false)
   {
     perror("you have uncommited changes!");
     return 1;
   }*/
  char filepath_dir1[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir1, ".neogit/branches/");
  strcat(filepath_dir1, branch);
  strcat(filepath_dir1, "/");
  strcat(filepath_dir1, tmp);
  FILE *file = fopen(filepath_dir1, "r");
  if (file == NULL)
    return 1;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, ".txt") != NULL)
    {
      char filepath[MAX_FILENAME_LENGTH];
      int last_commit_ID_file;
      sscanf(line, "%s %d", filepath, &last_commit_ID_file);
      char filepath_dir[MAX_FILENAME_LENGTH];
      strcpy(filepath_dir, ".neogit/files/");
      char tmp[10];
      sprintf(tmp, "%d", last_commit_ID_file);
      strcat(filepath_dir, tmp);
      if (is_samed_file(filepath, filepath_dir))
      {
        continue;
      }
      else
      {
        remove(filepath);
        copy_file(filepath_dir, filepath);
      }
    }
  }
  fclose(file);
  FILE *config2 = fopen(".neogit/config", "r");
  char line1[MAX_LINE_LENGTH];
  FILE *config1 = fopen(".neogit/config1", "w");
  if (config1 == NULL)
    return 1;
  while (fgets(line1, sizeof(line), config) != NULL)
  {
    if (strstr(line1, "current_commit_ID") == NULL)
    {
      fprintf(config1, "%s", line1);
    }
    else
    {
      fprintf(config1, "current_commit_ID: %s\n", tmp);
    }
  }
  fclose(config2);
  fclose(config1);
  remove(".neogit/config");
  rename(".neogit/config1", ".neogit/config");
  return 0;
}

int run_checkout_HEAD_n(int argc, char *const argv[])
{
  FILE *config = fopen(".neogit/config", "r");
  if (config == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  char branch[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), config) != NULL)
  {
    if (strstr(line, "branch") != NULL)
    {
      sscanf(line, "branch: %s", branch);
      break;
    }
  }
  fclose(config);
  char dir_path[MAX_FILENAME_LENGTH];
  strcpy(dir_path, ".neogit/branches/");
  strcat(dir_path, branch);
  char filepath_dir[MAX_FILENAME_LENGTH];

  DIR *dir2 = opendir(dir_path);
  struct dirent *entry2;
  if (dir2 == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  int ID[1000];
  int i = 0;
  while ((entry2 = readdir(dir2)) != NULL)
  {
    if (entry2->d_type == DT_REG)
    {
      ID[i] = atoi(entry2->d_name);
      i++;
    }
  }
  int n = atoi(argv[3][1]);
  if (n > i)
  {
    perror("you have not enough commits!");
    return 1;
  }
  int last_commit_ID = ID[i - n - 1];
  closedir(dir2);
  char tmp[10];
  sprintf(tmp, "%d", last_commit_ID);
  /* if (availabe_checkout(tmp) == false)
   {
     perror("you have uncommited changes!");
     return 1;
   }*/
  char filepath_dir1[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir1, ".neogit/branches/");
  strcat(filepath_dir1, branch);
  strcat(filepath_dir1, "/");
  strcat(filepath_dir1, tmp);
  FILE *file = fopen(filepath_dir1, "r");
  if (file == NULL)
    return 1;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strstr(line, ".txt") != NULL)
    {
      char filepath[MAX_FILENAME_LENGTH];
      int last_commit_ID_file;
      sscanf(line, "%s %d", filepath, &last_commit_ID_file);
      char filepath_dir[MAX_FILENAME_LENGTH];
      strcpy(filepath_dir, ".neogit/files/");
      char tmp[10];
      sprintf(tmp, "%d", last_commit_ID_file);
      strcat(filepath_dir, tmp);
      if (is_samed_file(filepath, filepath_dir))
      {
        continue;
      }
      else
      {
        remove(filepath);
        copy_file(filepath_dir, filepath);
      }
    }
  }
  fclose(file);
  FILE *config2 = fopen(".neogit/config", "r");
  char line1[MAX_LINE_LENGTH];
  FILE *config1 = fopen(".neogit/config1", "w");
  if (config1 == NULL)
    return 1;
  while (fgets(line1, sizeof(line), config) != NULL)
  {
    if (strstr(line1, "current_commit_ID") == NULL)
    {
      fprintf(config1, "%s", line1);
    }
    else
    {
      fprintf(config1, "current_commit_ID: %s\n", tmp);
    }
  }
  fclose(config2);
  fclose(config1);
  remove(".neogit/config");
  rename(".neogit/config1", ".neogit/config");
  return 0;
}

int run_tag(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  int x = 0;
  int message = 0;
  int commit_ID = 0;
  for (int i = 2; i < argc; i++)
  {
    if (strcmp(argv[i], "-f") == 0)
    {
      x = 1;
    }
    else if (strcmp(argv[i], "-m") == 0)
    {
      message = i + 1;
    }
    else if (strcmp(argv[i], "-c") == 0)
    {
      commit_ID = i + 1;
    }
  }
  if (argc > 2 && strcmp(argv[2], "-a") == 0)
  {
    char tag[MAX_LINE_LENGTH];
    strcpy(tag, argv[3]);
    DIR *dir = opendir(".neogit/tags");
    struct dirent *entry;
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_REG)
      {
        if (strcmp(entry->d_name, tag) == 0)
        {
          if (x == 0)
          {
            perror("tag already exists!");
            return 1;
          }
          else
          {
            remove(entry->d_name);
          }
        }
      }
    }
    closedir(dir);
    char author[MAX_LINE_LENGTH];
    char tag_dir[MAX_FILENAME_LENGTH];
    strcpy(tag_dir, ".neogit/tags/");
    strcat(tag_dir, tag);
    FILE *file = fopen(tag_dir, "w");
    if (file == NULL)
      return 1;
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/config");
    FILE *config = fopen(filepath_dir, "r");
    if (config == NULL)
      return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), config) != NULL)
    {
      if (strstr(line, "current_commit_ID") != NULL && commit_ID == 0)
      {
        sscanf(line, "current_commit_ID: %d", &commit_ID);
      }
      else if (strstr(line, "username") != NULL)
      {
        sscanf(line, "username: %s", author);
      }
      else if (strstr(line, "email") != NULL)
      {
        char email[MAX_LINE_LENGTH];
        sscanf(line, "email: %s", email);
        strcat(author, " <");
        strcat(author, email);
        strcat(author, ">");
      }
    }
    fclose(config);
    fprintf(file, "tag: %s\n", tag);
    fprintf(file, "commit_ID: %d\n", commit_ID);
    fprintf(file, "author: %s\n", author);
    fprintf(file, "date: %s %s\n", __DATE__, __TIME__);
    if (message != 0)
    {
      fprintf(file, "message: %s\n", argv[message]);
    }
    else
    {
      fprintf(file, "message: \n");
    }
    fclose(file);
  }
  else if (argc == 2)
  {
    DIR *dir = opendir(".neogit/tags");
    struct dirent *entry;
    if (dir == NULL)
    {
      perror("Error opening current directory");
      return 1;
    }
    printf("Tags:\n");
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_REG)
      {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
          printf(" -%s\n", entry->d_name);
        }
      }
    }
    closedir(dir);
  }
  else if (strcmp(argv[2], "show") == 0)
  {
    char tag_name[MAX_LINE_LENGTH];
    strcpy(tag_name, argv[3]);
    char path[MAX_FILENAME_LENGTH];
    strcpy(path, ".neogit/tags/");
    strcat(path, tag_name);
    FILE *file = fopen(path, "r");
    if (file == NULL)
      return 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, "message") == NULL)
      {
        printf("%s", line);
      }
      else if (strstr(line, "message") != NULL)
      {
        if (line[9] == '\n')
        {
          continue;
        }
        else
        {
          printf("%s", line);
        }
      }
    }
    fclose(file);
  }
  return 0;
}

int pre_commit_chek()
{

  DIR *dir = opendir(".neogit/staged");
  struct dirent *entry;
  if (dir == NULL)
  {
    perror("Error opening current directory");
    return 1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      printf("%s:\n", entry->d_name);
      FILE *file = fopen(".neogit/hooks_applied", "r");
      if (file == NULL)
      {
        perror("no hooks applied!");
        return 1;
      }
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        if (strstr(line, "todo-check") != NULL)
        {
          if (strstr(entry->d_name, ".txt") != NULL || strstr(entry->d_name, ".c") != NULL || strstr(entry->d_name, ".cpp") != NULL)
          {

            if (todo_check(entry->d_name) == true)
            {
              printf("todo-check.................................................................................................PASSED\n");
            }
            else
            {
              printf("todo-check.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("todo-check.................................................................................................SKIPPED\n");
          }
        }

        else if (strstr(line, "eof-blank-space") != NULL)
        {
          if (strstr(entry->d_name, ".txt") != NULL || strstr(entry->d_name, ".c") != NULL || strstr(entry->d_name, ".cpp") != NULL)
          {

            if (eof_blank_space(entry->d_name) == true)
            {
              printf("eof-blank-space.................................................................................................PASSED\n");
            }
            else
            {
              printf("eof-blank-space.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("eof-blank-space.................................................................................................SKIPPED\n");
          }
        }
        else if (strstr(line, "format-check") != NULL)
        {

          if (format_check(entry->d_name) == true)
          {
            printf("format-check.................................................................................................PASSED\n");
          }
          else
          {
            printf("format-check.................................................................................................FAILED\n");
          }
        }
        else if (strstr(line, "balance-braces") != NULL)
        {
          if (strstr(entry->d_name, ".c") != NULL || strstr(entry->d_name, ".cpp") != NULL || strstr(entry->d_name, ".txt") != NULL)
          {
            if (balance_braces(entry->d_name) == true)
            {
              printf("balance-braces.................................................................................................PASSED\n");
            }
            else
            {
              printf("balance-braces.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("balance-braces.................................................................................................SKIPPED\n");
          }
        }
        else if (strstr(line, "static-error-check") != NULL)
        {
          if (strstr(entry->d_name, ".c") != NULL || strstr(entry->d_name, ".cpp") != NULL)
          {
            if (static_error_check(entry->d_name) == true)
            {
              printf("static-error-check.................................................................................................PASSED\n");
            }
            else
            {
              printf("static-error-check.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("static-error-check.................................................................................................SKIPPED\n");
          }
        }
        else if (strstr(line, "file-size-check") != NULL)
        {
          if (file_size_check(entry->d_name) == true)
          {
            printf("file-size-check.................................................................................................PASSED\n");
          }
          else
          {
            printf("file-size-check.................................................................................................FAILED\n");
          }
        }
        else if (strstr(line, "character-limit") != NULL)
        {
          if (strstr(entry->d_name, ".txt") != NULL || strstr(entry->d_name, ".c") != NULL || strstr(entry->d_name, ".cpp") != NULL)
          {
            if (character_limit(entry->d_name) == true)
            {
              printf("character-limit.................................................................................................PASSED\n");
            }
            else
            {
              printf("character-limit.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("character-limit.................................................................................................SKIPPED\n");
          }
        }
        else if (strstr(line, "time-limit") != NULL)
        {
          if (strstr(entry->d_name, ".mp4") != NULL || strstr(entry->d_name, ".mp3") != NULL || strstr(entry->d_name, ".wav") != NULL)
          {
            if (time_limit(entry->d_name) == true)
            {
              printf("time-limit.................................................................................................PASSED\n");
            }
            else
            {
              printf("time-limit.................................................................................................FAILED\n");
            }
          }
          else
          {
            printf("time-limit.................................................................................................SKIPPED\n");
          }
        }
      }
    }
  }

  return 0;
}

bool todo_check(char *filepath)
{

  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);
  FILE *file = fopen(filepath_dir, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  if (strstr(filepath, ".txt") != NULL)
  {
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, "TODO") != NULL)
      {
        return false;
      }
    }
    return true;
  }
  else if (strstr(filepath, ".c") != NULL || strstr(filepath, ".cpp") != NULL)
  {
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, "//") != NULL && strstr(line, "TODO") != NULL)
      {
        return false;
      }
      else if (strstr(line, "/*") != NULL && strstr(line, "TODO") != NULL)
      {
        return false;
      }
    }
    FILE *file;
    char buffer[1000];
    size_t bytesRead;

    file = fopen("your_file.c", "rb");

    if (file == NULL)
    {
      perror("Error opening file");
      return 1;
    }

    bytesRead = fread(buffer, 1, sizeof(buffer) - 1, file);

    buffer[bytesRead] = '\0';

    char *startComment = strstr(buffer, "/*");
    char *endComment = strstr(buffer, "*/");

    if (startComment != NULL && endComment != NULL && startComment < endComment)
    {
      char *todoInComment = strstr(startComment, "TODO");

      if (todoInComment != NULL && todoInComment < endComment)
      {
        return false;
      }
    }

    // بستن فایل
    fclose(file);
    return true;
  }
}

bool eof_blank_space(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);
  FILE *file = fopen(filepath_dir, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (line[strlen(line) - 1] == '\n')
    {
      return false;
    }
    if (isspace(line[strlen(line) - 1]) == 0)
    {
      return false;
    }
  }
  fclose(file);
  return true;
}

bool format_check(char *filepath)
{
  if (strstr(filepath, ".c") != NULL || strstr(filepath, ".cpp") != NULL)
  {
    return true;
  }
  else if (strstr(filepath, ".txt") != NULL)
  {
    return true;
  }
  else if (strstr(filepath, ".h") != NULL)
  {
    return true;
  }
  else if (strstr(filepath, ".mp3") || strstr(filepath, ".mp4"))
  {
    return true;
  }
  else if (strstr(filepath, ".jpg"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool balance_braces(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);
  FILE *file = fopen(filepath_dir, "r");
  if (file == NULL)
    return 1;
  char line[MAX_LINE_LENGTH];
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;

  while (fgets(line, sizeof(line), file) != NULL)
  {
    for (int i = 0; i < strlen(line); i++)
    {
      if (line[i] == '{')
      {
        count1++;
      }
      else if (line[i] == '}')
      {
        count1--;
      }
      else if (line[i] == '(')
      {
        count2++;
      }
      else if (line[i] == ')')
      {
        count2--;
      }
      else if (line[i] == '[')
      {
        count3++;
      }
      else if (line[i] == ']')
      {
        count3--;
      }
    }
  }
  fclose(file);
  if (count1 == 0 && count2 == 0 && count3 == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool static_error_check(char *filepath)
{
  if (strstr(filepath, ".c") != NULL || strstr(filepath, ".cpp") != NULL)
  {
    opendir(".neogit/staged");

    char command[1024];
    char filename[MAX_FILENAME_LENGTH];
    strcpy(filename, filepath);
    strtok(filepath, ".");
    snprintf(command, sizeof(command), "gcc -o %s %s", filename, filepath);
    int compile_result = system(command);

    if (compile_result == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return true;
  }
}

bool file_size_check(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);
  FILE *file = fopen(filepath_dir, "r");
  if (file == NULL)
    return 1;
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fclose(file);
  // printf("%ld\n", size);
  if (size > 5 * 1024 * 1024)
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool character_limit(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);
  FILE *file = fopen(filepath_dir, "r");
  if (file == NULL)
    return 1;
  int count = 0;
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL)
  {
    count += strlen(line);
  }
  fclose(file);
  if (count > 20000)
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool time_limit(char *filepath)
{
  char filepath_dir[MAX_FILENAME_LENGTH];
  strcpy(filepath_dir, ".neogit/staged/");
  strcat(filepath_dir, filepath);

  char command[1024];
  FILE *fp;
  char duration_str[128];
  unsigned long long int duration = 0;

  sprintf(command, "ffprobe -i %s -show_entries format=duration -v quiet -of csv=\"p=0\" > output.txt", filepath_dir);
  system(command);

  fp = fopen("output.txt", "r");
  if (fp == NULL)
  {
    printf("Error opening file\n");
    return 1;
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), fp) != NULL)
  {
    //   printf("%s", line);
    strncpy(duration_str, line, sizeof(duration_str));
  }

  fclose(fp);

  duration_str[strcspn(duration_str, "\n")] = 0;

  duration = strtoull(duration_str, NULL, 10);

  // printf("Duration: %llu\n", duration);

  if (duration > MAX_DURATION)
  {

    return false;
  }
  else
  {

    return true;
  }
}

int run_pre_commit(int argc, char *const argv[])
{
  if (argc < 2)
  {
    perror("please use a correct format!");
    return 1;
  }
  if (argc == 2)
  {
    pre_commit_chek();
  }
  else
  {

    if (strcmp(argv[3], "list") == 0)
    {
      FILE *file = fopen(".neogit/hooks", "r");
      if (file == NULL)
        return 1;
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        printf("%s", line);
      }
      fclose(file);
    }
    else if (strcmp(argv[2], "applied") == 0)
    {
      FILE *file = fopen(".neogit/hooks_applied", "r");
      if (file == NULL)
        return 1;
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        printf("%s", line);
      }
      fclose(file);
    }
    else if (strcmp(argv[2], "add") == 0)
    {
      FILE *file = fopen(".neogit/hooks_applied", "a");
      if (file == NULL)
        return 1;
      fprintf(file, "%s\n", argv[4]);
      fclose(file);
    }
    else if (strcmp(argv[2], "remove") == 0)
    {
      FILE *file = fopen(".neogit/hooks_applied", "r");
      FILE *file1 = fopen(".neogit/hooks_applied1", "w");
      if (file == NULL)
        return 1;
      if (file1 == NULL)
        return 1;
      char line[MAX_LINE_LENGTH];
      while (fgets(line, sizeof(line), file) != NULL)
      {
        if (strstr(line, argv[4]) == NULL)
        {
          fprintf(file1, "%s", line);
        }
      }
      fclose(file);
      fclose(file1);
      remove(".neogit/hooks_applied");
      rename(".neogit/hooks_applied1", ".neogit/hooks_applied");
    }
  }
  return 0;
}
int pre_f(int argc, char *const argv)
{
  for (int i = 3; i < argc; i++)
  {
    char filepath_dir[MAX_FILENAME_LENGTH];
    char filepath[MAX_FILENAME_LENGTH];
    strcpy(filepath, ".neogit/staged/");
    strcat(filepath, argv[i]);
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
      perror("no such file!");
      break;
    }
    fclose(file);
    strcpy(filepath_dir, argv[i]);

    FILE *file1 = fopen(".neogit/hooks_applied", "r");
    if (file == NULL)
    {
      perror("no hooks applied!");
      return 1;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file1) != NULL)
    {
      if (strstr(line, "todo-check") == 0)
      {
        if (strstr(filepath_dir, ".txt") != NULL || strstr(filepath_dir, ".c") != NULL || strstr(filepath_dir, ".cpp") != NULL)
        {

          if (todo_check(filepath_dir) == true)
          {
            printf("todo-check.................................................................................................PASSED\n");
          }
          else
          {
            printf("todo-check.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("todo-check.................................................................................................SKIPPED\n");
        }
      }

      else if (strstr(line, "eof-blank-space") != NULL)
      {
        if (strstr(filepath_dir, ".txt") != NULL || strstr(filepath_dir, ".c") != NULL || strstr(filepath_dir, ".cpp") != NULL)
        {

          if (eof_blank_space(filepath_dir) == true)
          {
            printf("eof-blank-space.................................................................................................PASSED\n");
          }
          else
          {
            printf("eof-blank-space.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("eof-blank-space.................................................................................................SKIPPED\n");
        }
      }
      else if (strstr(line, "format-check") != NULL)
      {

        if (format_check(filepath_dir) == true)
        {
          printf("format-check.................................................................................................PASSED\n");
        }
        else
        {
          printf("format-check.................................................................................................FAILED\n");
        }
      }
      else if (strstr(line, "balance-braces") != NULL)
      {
        if (strstr(filepath_dir, ".c") != NULL || strstr(filepath_dir, ".cpp") != NULL || strstr(filepath_dir, ".txt") != NULL)
        {
          if (balance_braces(filepath_dir) == true)
          {
            printf("balance-braces.................................................................................................PASSED\n");
          }
          else
          {
            printf("balance-braces.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("balance-braces.................................................................................................SKIPPED\n");
        }
      }
      else if (strstr(line, "static-error-check") != NULL)
      {
        if (strstr(filepath_dir, ".c") != NULL || strstr(filepath_dir, ".cpp") != NULL)
        {
          if (static_error_check(filepath_dir) == true)
          {
            printf("static-error-check.................................................................................................PASSED\n");
          }
          else
          {
            printf("static-error-check.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("static-error-check.................................................................................................SKIPPED\n");
        }
      }
      else if (strstr(line, "file-size-check") != NULL)
      {
        if (file_size_check(filepath_dir) == true)
        {
          printf("file-size-check.................................................................................................PASSED\n");
        }
        else
        {
          printf("file-size-check.................................................................................................FAILED\n");
        }
      }
      else if (strstr(line, "character-limit") != NULL)
      {
        if (strstr(filepath_dir, ".txt") != NULL || strstr(filepath_dir, ".c") != NULL || strstr(filepath_dir, ".cpp") != NULL)
        {
          if (character_limit(filepath_dir) == true)
          {
            printf("character-limit.................................................................................................PASSED\n");
          }
          else
          {
            printf("character-limit.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("character-limit.................................................................................................SKIPPED\n");
        }
      }
      else if (strstr(line, "time-limit") != NULL)
      {
        if (strstr(filepath_dir, ".mp4") != NULL || strstr(filepath_dir, ".mp3") != NULL || strstr(filepath_dir, ".wav") != NULL)
        {
          if (time_limit(filepath_dir) == true)
          {
            printf("time-limit.................................................................................................PASSED\n");
          }
          else
          {
            printf("time-limit.................................................................................................FAILED\n");
          }
        }
        else
        {
          printf("time-limit.................................................................................................SKIPPED\n");
        }
      }
    }
  }
  return 0;
}

int run_grep(int argc, char *const argv[])
{
  if (argc < 3)
  {
    perror("please use a correct format!");
    return 1;
  }
  int c, n;
  for (int i = 3; i < argc; i++)
  {
    if (strcmp(argv[i], "-c") == 0)
    {
      c = i + 1;
    }
    else if (strcmp(argv[i], "-n") == 0)
    {
      n = i;
    }
  }
  // printf("%d %d",c,n);
  if (c == 0 && n == 0)
  {
    char filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    strcpy(word, argv[5]);
    strcpy(filename, argv[3]);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
      return 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, word) != NULL)
      {
        printf("%s", line);
      }
    }
    fclose(file);
  }
  else if (c != 0 && n == 0)
  {
    char filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    char commit_ID[MAX_LINE_LENGTH];
    strcpy(commit_ID, argv[c]);
    strcpy(word, argv[5]);
    strcpy(filename, ".neogit/files/");
    strcat(filename, argv[3]);
    strcat(filename, "/");
    strcat(filename, commit_ID);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
      return 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
      if (strstr(line, word) != NULL)
      {
        printf("%s", line);
      }
    }
    fclose(file);
  }
  else if (c == 0 && n != 0)
  {
    char filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    strcpy(word, argv[5]);
    strcpy(filename, argv[3]);
    int i = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
      return 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
      i++;
      if (strstr(line, word) != NULL)
      {
        printf("%d: %s", i, line);
      }
    }
    fclose(file);
  }
  else if (c != 0 && n != 0)
  {
    char filename[MAX_FILENAME_LENGTH];
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    char commit_ID[MAX_LINE_LENGTH];
    strcpy(commit_ID, argv[c]);
    strcpy(word, argv[5]);
    strcpy(filename, ".neogit/files/");
    strcat(filename, argv[3]);
    strcat(filename, "/");
    strcat(filename, commit_ID);
    int i = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
      return 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
      i++;
      if (strstr(line, word) != NULL)
      {
        printf("%d: %s", i, line);
      }
    }
    fclose(file);
  }
  return 0;
}
// #define _DEB
#ifdef _DEB
int main()
{
  int argc = 4;
  char *argv[] = {"neogit", "tag", "-a", "v1.0a"};
  // printf("%s", argv[1]);
#else
int main(int argc, char *argv[])
{
#endif

  if (argc < 2)
  {
    fprintf(stdout, "please enter a valid command");
    return 1;
  }

  //  printf("%d", argc);
  print_command(argc, argv);
  int alais = is_alias(argc, argv);
  if (exists_neogit() == true || strcmp(argv[1], "init") == 0 || strcmp(argv[1], "config") == 0)
  {
    if (alais == 1)
    {

      if (strcmp(argv[1], "init") == 0)
      {

        return run_init(argc, argv);
      }
      else if (strcmp(argv[1], "config") == 0)
      {
        if (strstr(argv[2], "alias") == NULL)
        {
          if (!is_config())
          {

            run_config_user(argc, argv);
            return create_configs();
          }
          else if (is_config())
          {
            return run_config_user(argc, argv);
          }
        }
        else
        {
          save_alias(argc, argv);
        }
      }
      else if (strcmp(argv[1], "add") == 0)
      {

        if (!is_config())
        {
          perror("please config your user!");
          return 1;
        }
        else if (is_config())
        {
          if (argv[2][0] == '-' && argv[2][1] == 'n')
          {
            int depth = (argv[3][0] - '0');
            // printf("%d", depth);

            const char *start_path = ".";

            OutputNode *rootNode = (OutputNode *)malloc(sizeof(OutputNode));
            rootNode->name = strdup("Root");
            rootNode->status = strdup("");
            rootNode->child = NULL;
            rootNode->next = NULL;

            add_depth(argc, argv, depth, start_path, rootNode);

            //    چاپ خروجی‌ها به صورت شاخه‌ای
            printOutput(rootNode, 0, 1);

            // add_depth(argc , argv , depth , ".");
          }
          else if (argv[2][0] == '-' && argv[2][1] == 'r')
          {
            return run_add_redo(argc, argv);
          }
          else
          {
            return run_add(argc, argv);
          }
        }
      }
      else if (strcmp(argv[1], "reset") == 0)
      {
        if (!is_config())
        {
          perror("please config your user!");
          return 1;
        }
        else if (is_config())
        {
          if (strcmp(argv[2], "-undo") == 0)
          {
            return reset_undo(argc, argv);
          }

          return run_reset(argc, argv);
        }
      }
      else if (strcmp(argv[1], "commit") == 0)
      {

        if (!is_config())
        {
          perror("please config your user!");
          return 1;
        }
        else if (is_config())
        {
          if (argc == 3 && argv[2][0] == '-' && argv[2][1] == 'm')
          {
            perror("please enter a message!");
            return 1;
          }
          else if (strlen(argv[3]) > 72)
          {

            perror("message is too long");
            return 1;
          }
          else if (argc > 4)
          {
            printf("if your message has more than one word, please use double quotes");
            return 1;
          }

          return run_commit(argc, argv);
        }
      }
      else if (strcmp(argv[1], "set") == 0)
      {
        return run_set(argc, argv);
      }
      else if (strcmp(argv[1], "replace") == 0)
      {
        return run_replace(argc, argv);
      }
      else if (strcmp(argv[1], "remove") == 0)
      {
        return run_remove(argc, argv);
      }
      else if (strcmp(argv[1], "log") == 0)
      {

        if (argc == 2)
        {
          return run_log(argc, argv);
        }
        else if (argv[2][0] == '-' && argv[2][1] == 'n')
        {
          return run_log(argc, argv);
        }
        else if (strcmp(argv[2], "-branch") == 0)
        {
          return run_log_branch(argc, argv);
        }
        else if (strcmp(argv[2], "-author") == 0)
        {
          return run_log_author(argc, argv);
        }
        else if (strcmp(argv[2], "-since") == 0 || strcmp(argv[2], "-befor") == 0)
        {
          return run_log_date(argc, argv);
        }
        else if (strcmp(argv[2], "-search") == 0)
        {
          return run_log_word(argc, argv);
        }
      }

      else if (strcmp(argv[1], "checkout") == 0)
      {
        if (!is_config())
        {
          perror("please config your user!");
          return 1;
        }
        else if (is_config())
        {
          if ('1' <= argv[2][0] && argv[2][0] <= '9')
          {
            return run_checkout_commit(argc, argv);
          }
          else if (strcmp(argv[2], "HEAD") == 0)
          {

            if (argc == 4)
            {
              return run_checkout_HEAD_n(argc, argv);
            }
            return run_checkout_HEAD();
          }
          else
          {
            return run_checkout_branch(argc, argv);
          }
        }
      }
      else if (strcmp(argv[1], "status") == 0)
      {
        return run_status(argc, argv);
      }
      else if (strcmp(argv[1], "branch") == 0)
      {
        return run_branch(argc, argv);
      }
      else if (strcmp(argv[1], "tag") == 0)
      {
        return run_tag(argc, argv);
      }
      else if (strcmp(argv[1], "pre-commit") == 0)
      {
        if (argc > 2)
        {
          if (strcmp(argv[2], "-f") == 0)
          {
            return pre_f(argc, argv);
          }
        }
        return run_pre_commit(argc, argv);
      }
      else if (strcmp(argv[1], "grep") == 0)
      {
        return run_grep(argc, argv);
      }
      else
      {
        perror("please enter a valid command");
        return 1;
      }
    }
  }
  else
  {
    perror("please init your repository!");
    return 1;
  }
}
