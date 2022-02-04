#include <yed/plugin.h>
#include <yed/syntax.h>

static yed_syntax syn;


#define _CHECK(x, r)                                                      \
do {                                                                      \
    if (x) {                                                              \
        LOG_FN_ENTER();                                                   \
        yed_log("[!] " __FILE__ ":%d regex error for '%s': %s", __LINE__, \
                r,                                                        \
                yed_syntax_get_regex_err(&syn));                          \
        LOG_EXIT();                                                       \
    }                                                                     \
} while (0)

#define SYN()          yed_syntax_start(&syn)
#define ENDSYN()       yed_syntax_end(&syn)
#define APUSH(s)       yed_syntax_attr_push(&syn, s)
#define APOP(s)        yed_syntax_attr_pop(&syn)
#define RANGE(r)       _CHECK(yed_syntax_range_start(&syn, r), r)
#define ONELINE()      yed_syntax_range_one_line(&syn)
#define SKIP(r)        _CHECK(yed_syntax_range_skip(&syn, r), r)
#define ENDRANGE(r)    _CHECK(yed_syntax_range_end(&syn, r), r)
#define REGEX(r)       _CHECK(yed_syntax_regex(&syn, r), r)
#define REGEXSUB(r, g) _CHECK(yed_syntax_regex_sub(&syn, r, g), r)
#define KWD(k)         yed_syntax_kwd(&syn, k)

#ifdef __APPLE__
#define WB "[[:>:]]"
#else
#define WB "\\b"
#endif

void estyle(yed_event *event)   { yed_syntax_style_event(&syn, event);         }
void ebuffdel(yed_event *event) { yed_syntax_buffer_delete_event(&syn, event); }
void ebuffmod(yed_event *event) { yed_syntax_buffer_mod_event(&syn, event);    }
void eline(yed_event *event)  {
    yed_frame *frame;

    frame = event->frame;

    if (!frame
    ||  !frame->buffer
    ||  frame->buffer->kind != BUFF_KIND_FILE
    ||  frame->buffer->ft != yed_get_ft("Rust")) {
        return;
    }

    yed_syntax_line_event(&syn, event);
}


void unload(yed_plugin *self) {
    yed_syntax_free(&syn);
#if YED_VERSION < 1400
    ys->redraw = 1;
#endif
}

int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler style;
    yed_event_handler buffdel;
    yed_event_handler buffmod;
    yed_event_handler line;


    YED_PLUG_VERSION_CHECK();

    yed_plugin_set_unload_fn(self, unload);

    style.kind = EVENT_STYLE_CHANGE;
    style.fn   = estyle;
    yed_plugin_add_event_handler(self, style);

    buffdel.kind = EVENT_BUFFER_PRE_DELETE;
    buffdel.fn   = ebuffdel;
    yed_plugin_add_event_handler(self, buffdel);

    buffmod.kind = EVENT_BUFFER_POST_MOD;
    buffmod.fn   = ebuffmod;
    yed_plugin_add_event_handler(self, buffmod);

    line.kind = EVENT_LINE_PRE_DRAW;
    line.fn   = eline;
    yed_plugin_add_event_handler(self, line);


    SYN();
        APUSH("&code-comment");
            RANGE("/\\*");
            ENDRANGE(  "\\*/");
            RANGE("//");
                ONELINE();
            ENDRANGE("$");
        APOP();

        APUSH("&code-string");
            REGEX("'(\\\\.|[^'\\\\])'");

            RANGE("\""); SKIP("\\\\\"");
                APUSH("&code-escape");
                    REGEX("\\\\.");
                APOP();
            ENDRANGE("\"");
        APOP();

        APUSH("&code-fn-call");
            REGEXSUB("([[:alpha:]_][[:alnum:]_]*!\{0,1})[[:space:]]*\\(", 1);
            REGEXSUB("([[:alpha:]_][[:alnum:]_]*!)[[:space:]]*\\[", 1);
        APOP();

        APUSH("&code-number");
            REGEXSUB("(^|[^[:alnum:]_])(-?([[:digit:]]+\\.[[:digit:]]*)|(([[:digit:]]*\\.[[:digit:]]+))(e\\+[[:digit:]]+)?[fFlL]?)"WB, 2);
            REGEXSUB("(^|[^[:alnum:]_])(-?[[:digit:]]+(([uU]?[lL]{0,2})|([lL]{0,2}[uU]?))?)"WB, 2);
            REGEXSUB("(^|[^[:alnum:]_])(0[xX][0-9a-fA-F]+(([uU]?[lL]{0,2})|([lL]{0,2}[uU]?))?)"WB, 2);
        APOP();

        APUSH("&code-keyword");
            KWD("as");
            KWD("const");
            KWD("crate");
            KWD("enum");
            KWD("extern");
            KWD("false");
            KWD("fn");
            KWD("for");
            KWD("impl");
            KWD("in");
            KWD("let");
            KWD("match");
            KWD("move");
            KWD("mut");
            KWD("pub");
            KWD("ref");
            KWD("self");
            KWD("Self");
            KWD("static");
            KWD("struct");
            KWD("super");
            KWD("trait");
            KWD("true");
            KWD("type");
            KWD("unsafe");
            KWD("use");
            KWD("where");
            KWD("while");
            KWD("async");
            KWD("await");
            KWD("abstract");
            KWD("become");
            KWD("box");
            KWD("final");
            KWD("macro");
            KWD("override");
            KWD("priv");
            KWD("typedef");
            KWD("unsized");
            KWD("virtual");
            KWD("yield");
        APOP();

        APUSH("&code-control-flow");
            KWD("break");
            KWD("case");
            KWD("continue");
            KWD("default");
            KWD("do");
            KWD("else");
            KWD("for");
            KWD("goto");
            KWD("if");
            KWD("loop");
            KWD("return");
            KWD("switch");
            KWD("while");
        APOP();

        APUSH("&code-typename");
            KWD("bool");
            KWD("char");
            KWD("i8");
            KWD("i16");
            KWD("i32");
            KWD("i64");
            KWD("i128");
            KWD("isize");
            KWD("u8");
            KWD("u16");
            KWD("u32");
            KWD("u64");
            KWD("u128");
            KWD("usize");
            KWD("array");
            KWD("slice");
            KWD("str");
            KWD("tuple");
            KWD("f32");
            KWD("f64");
        APOP();

        APUSH("&code-constant");
        APOP();

        APUSH("&code-field");
            REGEXSUB("(\\.|->)[[:space:]]*([[:alpha:]_][[:alnum:]_]*)", 2);
        APOP();
    ENDSYN();

#if YED_VERSION < 1400
    ys->redraw = 1;
#endif

    return 0;
}
