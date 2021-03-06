! function(t) {
    var e = {};

    function r(n) {
        if (e[n]) return e[n].exports;
        var o = e[n] = {
            i: n,
            l: !1,
            exports: {}
        };
        return t[n].call(o.exports, o, o.exports, r), o.l = !0, o.exports
    }
    r.m = t, r.c = e, r.d = function(t, e, n) {
        r.o(t, e) || Object.defineProperty(t, e, {
            enumerable: !0,
            get: n
        })
    }, r.r = function(t) {
        "undefined" !== typeof Symbol && Symbol.toStringTag && Object.defineProperty(t, Symbol.toStringTag, {
            value: "Module"
        }), Object.defineProperty(t, "__esModule", {
            value: !0
        })
    }, r.t = function(t, e) {
        if (1 & e && (t = r(t)), 8 & e) return t;
        if (4 & e && "object" === typeof t && t && t.__esModule) return t;
        var n = Object.create(null);
        if (r.r(n), Object.defineProperty(n, "default", {
                enumerable: !0,
                value: t
            }), 2 & e && "string" != typeof t)
            for (var o in t) r.d(n, o, function(e) {
                return t[e]
            }.bind(null, o));
        return n
    }, r.n = function(t) {
        var e = t && t.__esModule ? function() {
            return t.default
        } : function() {
            return t
        };
        return r.d(e, "a", e), e
    }, r.o = function(t, e) {
        return Object.prototype.hasOwnProperty.call(t, e)
    }, r.p = "/static/", r(r.s = 4)
}({
    "./app/js/matchers/GoLang.js": function(t, e) {
        importScripts("/static/golang.js");
        t = Module();
        self.onmessage = function(e) {
            self.postMessage("onload");
            var i = new Date,
                u = e.data,
                s = u.regex,
                g = u.flags,
                l = u.testString,
                c = g.replace("g", ""),
                f = s;
            c.length > 0 && (f = "(?".concat(c, ")").concat(s));
            var p = function(e, i, u) {
                var s = -1 !== i.indexOf("g"),
                    g = [];
                if (o.pattern !== e || o.flags !== i || !o.regexPointer) {
                    o.regexPointer && t._free(o.regexPointer);
                    var l = 2 * (2 * e.length + 1),
                        c = t._malloc(l);
                    t.stringToUTF8(e, c, l), o.pattern = e, o.flags = i, o.regexPointer = n(c)
                }
                if (o.subject !== u || !o.testStringPointer) {
                    o.testStringPointer && t._free(o.testStringPointer);
                    var f = 2 * (2 * u.length + 1),
                        p = t._malloc(f);
                    t.stringToUTF8(u, p, f), o.subject = u, o.testStringPointer = p, o.subjectLength = function(e, r) {
                        for (var n = 0, o = e, a = e + r; o < a; o++) {
                            if (0 === t.HEAP8[o]) return n;
                            n++
                        }
                        return n
                    }(p, f)
                }
                var b = 0,
                    d = 0;
                for (; d <= o.subjectLength;) {
                    var v = r(o.regexPointer, o.testStringPointer, d, o.subjectLength);
                    if (!v.get(0)) break;
                    var P = v.get(0).getEnd(),
                        j = v.get(0).getStart();
                    if (d = j === P ? P + 1 : P, g.push(a(v, b, u, o.testStringPointer)), b++, !s) break
                }
                return {
                    matchResult: g
                }
            }(f, g, l);
            p.time = new Date - i, self.postMessage(p)
        };
        var r = t.RE2_Match,
            n = t.RE2_CreateRegex,
            o = {
                pattern: null,
                flags: null,
                regexPointer: null,
                subject: null,
                testStringPointer: null,
                subjectLength: 0
            };

        function a(t, e, r, n) {
            for (var o = [], a = 0, u = t.size(); a < u; a++) {
                var s = t.get(a),
                    g = s.getStart(),
                    l = s.getEnd(),
                    c = i(n, n + g),
                    f = c + i(n + g, n + l);
                o.push({
                    isParticipating: !0,
                    groupNum: a,
                    start: c,
                    end: f,
                    content: r.substring(c, f),
                    match: e
                })
            }
            return o
        }

        function i(e, r) {
            for (var n = 0; e !== r;) {
                var o = 255 & t.HEAP8[e];
                if (0 === o) return n;
                o < 240 ? (o < 128 ? e++ : e += o < 224 ? 2 : 3, n++) : (e += 4, n += 2)
            }
            return n
        }
    },
    4: function(t, e, r) {
        t.exports = r("./app/js/matchers/GoLang.js")
    }
});
