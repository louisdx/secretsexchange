var TheP = str2bigInt("124325339146889384540494091085456630009856882741872806181731279018491820800119460022367403769795008250021191767583423221479185609066059226301250167164084041279837566626881119772675984258163062926954046545485368458404445166682380071370274810671501916789361956272226105723317679562001235501455748016154805420913", 10, 1024, 64);
var TheG = str2bigInt("115740200527109164239523414760926155534485715860090261532154107313946218459149402375178179458041461723723231563839316251515439564315555249353831328479173170684416728715378198172203100328308536292821245983596065287318698169565702979765910089654821728828592422299160041156491980943427556153020487552135890973413", 10, 1024, 64);

function ExpMod(Base, Expo, Modu) {
    return powMod(Base, Expo, Modu);
}

function compute(gx, px, nx) {
    var g = str2bigInt(gx, 10, 1024, 64);
    var p = str2bigInt(px, 10, 1024, 64);
    var n = str2bigInt(nx, 10, 1024, 64);

    //return res = (g.pow(n)).mod(p);
    return ExpMod(g, n, p);
}

function randomBigNum(elem)
{
    elem.value = bigInt2str(randBigInt(512, 1), 10);
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
