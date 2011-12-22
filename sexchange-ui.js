var in_a, in_b, in_ga, in_gb, in_p, in_g;
var in_put_a, input_b, out_put, out_gab;
var but_a, but_b, but_ra, but_rb, but_rnda, but_rndb, but_enc, but_dec;

var limit = int2bigInt(2, 8, 1);
var tfs   = int2bigInt(256, 8, 1);

function Init() {
    if (document.getElementById) {
        in_a    = document.getElementById("in_a");
        in_b    = document.getElementById("in_b");
        in_ga   = document.getElementById("in_ga");
        in_gb   = document.getElementById("in_gb");
        in_g    = document.getElementById("in_g");
        in_p    = document.getElementById("in_p");

        in_put_a  = document.getElementById("in_put_a");
        in_put_b  = document.getElementById("in_put_b");
        out_put = document.getElementById("out_put");
        out_gab = document.getElementById("out_gab");

        but_a    = document.getElementById("go_a");
        but_b    = document.getElementById("go_b");
        but_ra   = document.getElementById("but_ra");
        but_rb   = document.getElementById("but_rb");
        but_rnda = document.getElementById("but_rnda");
        but_rndb = document.getElementById("but_rndb");
        but_enc  = document.getElementById("go_enc");
        but_dec  = document.getElementById("go_dec");
    }

    if (but_a) {
        but_a.addEventListener("click", runAlice, false);
    }
    if (but_b) {
        but_b.addEventListener("click", runBob, false);
    }
    if (but_ra) {
        but_ra.addEventListener("click", fillAliceInput, false);
    }
    if (but_rb) {
        but_rb.addEventListener("click", fillBobInput, false);
    }

    if (but_rnda && in_a) {
        but_rnda.addEventListener("click", function(){ randomBigNum(in_a); }, false);
    }
    if (but_rndb && in_b) {
        but_rndb.addEventListener("click", function(){ randomBigNum(in_b); }, false);
    }

    if (but_enc) {
        but_enc.addEventListener("click", Encrypt, false);
    }

    if (but_dec) {
        but_dec.addEventListener("click", Decrypt, false);
    }

    in_g.value = bigInt2str(TheG, 10);
    in_p.value = bigInt2str(TheP, 10);
}

window.addEventListener("load", Init, false);

function fillAliceInput(ev) {
    in_gb.value = in_put_a.value;
    out_gab.value = bigInt2str(compute(in_gb.value, in_p.value, in_a.value), 10);
}

function fillBobInput(ev) {
    in_ga.value = in_put_b.value;
}

function runAlice(ev) {
    in_ga.value = bigInt2str(compute(in_g.value, in_p.value, in_a.value), 10);
    out_put.value = in_ga.value;
}

function runBob(ev) {
    in_gb.value = bigInt2str(compute(in_g.value, in_p.value, in_b.value), 10);
    out_gab.value = bigInt2str(compute(in_ga.value, in_p.value, in_b.value), 10);
    out_put.value = in_gb.value;
}

function Encrypt(ev) {
    var msg = document.getElementById("in_pt").value;
    var secret = str2bigInt(out_gab.value, 10, 1024, 64);

    var r = str2bigInt("0", 10, 1024, 64);
    var pad = 100 - msg.length;

    for (var i = 0; i < 100; ++i) {
        var v = ((i < msg.length) ? msg.charCodeAt(i) : pad);
        r = mult(r, tfs);
        var b = modInt(secret, 256);
        r = addInt(r, v ^ b);
        rightShift_(secret, 8);
    }

    document.getElementById("in_et").value = bigInt2str(r, 10);
}

function Decrypt(ev) {
    var msg = str2bigInt(document.getElementById("in_et").value, 10, 1024, 64);
    var secret = str2bigInt(out_gab.value, 10, 1024, 64);

    var raw = Object();
    for (var i = 0; i < 100; ++i) {
        raw[i] = modInt(msg, 256);
        rightShift_(msg, 8);
    }

    var r = "";
    var pad;
    for (var i = 0; i < 100; ++i) {
        pad = raw[100 - i - 1] ^ modInt(secret, 256);
        r += String.fromCharCode(pad);
        rightShift_(secret, 8);
    }

    document.getElementById("in_pt").value = r.substring(0, 100 - pad);
}
