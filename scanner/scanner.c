#include "src/scanner.c"

static void scanner_write_type(FILE *output, struct arg *a)
{
    switch (a->type) {
    default:
    case INT:
    case FD:
        fprintf(output, "int32_t ");
        break;
    case NEW_ID:
    case UNSIGNED:
        fprintf(output, "uint32_t ");
        break;
    case FIXED:
        fprintf(output, "wl_fixed_t ");
        break;
    case STRING:
        fprintf(output, "const char *");
        break;
    case OBJECT:
        fprintf(output, "struct %s *", a->interface_name);
        break;
    case ARRAY:
        fprintf(output, "struct wl_array *");
        break;
    }
}

void scanner_write_args(FILE *output, struct wl_list *arg_list, int methods)
{
    struct arg *a;
    int first = 1;

    wl_list_for_each(a, arg_list, link)
    {
        if (!first)
            fprintf(output, ", ");

        if (a->type == OBJECT || (a->type == NEW_ID && methods))
            fprintf(output, "struct wl_resource *");
        else
            scanner_write_type(output, a);

        fprintf(output, "%s", a->name);

        first = 0;
    }
}

void scanner_write_method(FILE *output, struct message *message)
{
    fprintf(output, "    void send_%s(", message->name);
    //fprintf(output, "struct wl_client *client, struct wl_resource *resource");
    //if (message->arg_count > 0)
    //    fprintf(output, ",\n        ");
    scanner_write_args(output, &message->arg_list, 1);
    fprintf(output, ");\n");
}

void scanner_write_signal(FILE *output, struct message *message)
{
    fprintf(output, "    were_signal<void (");
    //fprintf(output, "struct wl_client *client, struct wl_resource *resource");
    //if (message->arg_count > 0)
    //    fprintf(output, ", ");
    scanner_write_args(output, &message->arg_list, 0);
    fprintf(output, ")> %s;\n", message->name);
}

void scanner_write_header(struct interface *i)
{
    struct message *message;
    char file_name[64];
    FILE *output;

    snprintf(file_name, 64, "sparkle_%s.h", i->name);
    output = fopen(file_name, "w");
    if (output == NULL)
        return;

    fprintf(output, "#ifndef SPARKLE_%s_H\n", i->uppercase_name);
    fprintf(output, "#define SPARKLE_%s_H\n\n", i->uppercase_name);

    fprintf(output, "#include \"sparkle_resource.h\"\n");
    fprintf(output, "\n");

    fprintf(output, "class sparkle_%s : public sparkle_resource\n{\n", i->name);

    fprintf(output, "public:\n");

    fprintf(output, "    ~sparkle_%s() override;\n", i->name);
    fprintf(output, "    sparkle_%s(struct wl_client *client, int version, uint32_t id);\n", i->name);
    fprintf(output, "    explicit sparkle_%s(struct wl_resource *resource);\n\n", i->name);

    wl_list_for_each(message, &i->event_list, link)
        scanner_write_method(output, message);
    fprintf(output, "\n");

    fprintf(output, "signals:\n");
    wl_list_for_each(message, &i->request_list, link)
        scanner_write_signal(output, message);
    fprintf(output, "\n");

    fprintf(output, "};\n\n");

    fprintf(output, "#endif // SPARKLE_%s_H\n", i->uppercase_name);

    fclose(output);
}

void scanner_write_bouncers(FILE *output, struct interface *i)
{
    struct message *message;

    if (wl_list_empty(&i->request_list))
        return;

    fprintf(output, "static const struct %s_interface interface = {\n", i->name);

    wl_list_for_each(message, &i->request_list, link)
    {
        if (message->destructor)
            fprintf(output, "    BOUNCER_2_D(sparkle_%s, %s),\n", i->name, message->name);
        else
            fprintf(output, "    BOUNCER_2(sparkle_%s, %s),\n", i->name, message->name);
    }

    fprintf(output, "};\n\n");
}

void scanner_write_args_1(FILE *output, struct wl_list *arg_list, int arg_count)
{
    struct arg *a;
    int first = 1;

    wl_list_for_each(a, arg_list, link)
    {
        if (!first)
            fprintf(output, ", ");

        fprintf(output, "%s", a->name);

        first = 0;
    }
}

void scanner_write_method_implementation(FILE *output, struct interface *i, struct message *message)
{
    fprintf(output, "void sparkle_%s::send_%s(", i->name, message->name);
    //fprintf(output, "struct wl_client *client, struct wl_resource *resource");
    //if (message->arg_count > 0)
    //    fprintf(output, ",\n        ");
    scanner_write_args(output, &message->arg_list, 1);
    fprintf(output, ")\n{\n");
    fprintf(output, "    if (valid())\n");
    fprintf(output, "        %s_send_%s(", i->name, message->name);
    fprintf(output, "resource()");
    if (message->arg_count > 0)
        fprintf(output, ", ");
    scanner_write_args_1(output, &message->arg_list, message->arg_count);
    fprintf(output, ");\n");
    fprintf(output, "}\n");
}

void scanner_write_source(struct interface *i)
{
    struct message *message;
    char file_name[64];
    FILE *output;

    snprintf(file_name, 64, "sparkle_%s.cpp", i->name);
    output = fopen(file_name, "w");
    if (output == NULL)
        return;

    fprintf(output, "#include \"sparkle_%s.h\"\n", i->name);
    fprintf(output, "#include \"sparkle_bouncer.h\"\n");
    fprintf(output, "\n");

    scanner_write_bouncers(output, i);

    fprintf(output, "sparkle_%s::~sparkle_%s()\n{\n", i->name, i->name);
    fprintf(output, "}\n\n");

    fprintf(output, "sparkle_%s::sparkle_%s(struct wl_client *client, int version, uint32_t id) :\n", i->name, i->name);

    if (wl_list_empty(&i->request_list))
        fprintf(output, "    sparkle_resource(client, &%s_interface, version, id, nullptr)\n{\n", i->name);
    else
        fprintf(output, "    sparkle_resource(client, &%s_interface, version, id, &interface)\n{\n", i->name);

    fprintf(output, "}\n\n");

    fprintf(output, "sparkle_%s::sparkle_%s(struct wl_resource *resource) :\n", i->name, i->name);
    fprintf(output, "    sparkle_resource(resource)\n{\n");

    fprintf(output, "}\n");

    wl_list_for_each(message, &i->event_list, link)
    {
        fprintf(output, "\n");
        scanner_write_method_implementation(output, i, message);
    }

    fclose(output);
}

int required(const char *name, int rc, char * const *rv)
{
    for (int i = 0; i < rc; ++i)
    {
        if (strcmp(rv[i], name) == 0)
            return 1;
    }

    return 0;
}

void scanner_write(struct protocol *protocol, int rc, char * const *rv)
{
    struct interface *i;

    wl_list_for_each(i, &protocol->interface_list, link)
    {
        if (required(i->name, rc, rv) == 1)
        {
            scanner_write_header(i);
            scanner_write_source(i);
        }
    }
}

int main(int argc, char *argv[])
{
    struct parse_context ctx;
    struct protocol protocol;
    FILE *input = stdin;
    char *input_filename = NULL;
    int len;
    void *buf;

    if (argc < 3)
        return -1;

    input_filename = argv[1];
    input = fopen(input_filename, "r");
    if (input == NULL) {
        fprintf(stderr, "Could not open input file: %s\n",
            strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* initialize protocol structure */
    memset(&protocol, 0, sizeof protocol);
    wl_list_init(&protocol.interface_list);
    protocol.core_headers = false;

    /* initialize context */
    memset(&ctx, 0, sizeof ctx);
    ctx.protocol = &protocol;
    ctx.loc.filename = input_filename;

    /* create XML parser */
    ctx.parser = XML_ParserCreate(NULL);
    XML_SetUserData(ctx.parser, &ctx);
    if (ctx.parser == NULL) {
        fprintf(stderr, "failed to create parser\n");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    XML_SetElementHandler(ctx.parser, start_element, end_element);
    XML_SetCharacterDataHandler(ctx.parser, character_data);

    do {
        buf = XML_GetBuffer(ctx.parser, XML_BUFFER_SIZE);
        len = fread(buf, 1, XML_BUFFER_SIZE, input);
        if (len < 0) {
            fprintf(stderr, "fread: %m\n");
            fclose(input);
            exit(EXIT_FAILURE);
        }
        if (XML_ParseBuffer(ctx.parser, len, len == 0) == 0) {
            fprintf(stderr,
                "Error parsing XML at line %ld col %ld: %s\n",
                XML_GetCurrentLineNumber(ctx.parser),
                XML_GetCurrentColumnNumber(ctx.parser),
                XML_ErrorString(XML_GetErrorCode(ctx.parser)));
            fclose(input);
            exit(EXIT_FAILURE);
        }
    } while (len > 0);

    XML_ParserFree(ctx.parser);

    if (chdir(argv[2]) != -1)
        scanner_write(&protocol, argc - 3, &argv[3]);


    struct interface *i;
    wl_list_for_each(i, &protocol.interface_list, link) {
        free_interface(i);
    }

    free_protocol(&protocol);
    fclose(input);

    return 0;
}


