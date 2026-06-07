const menu = document.getElementById("contextMenu");

document.querySelector(".codeEditor").addEventListener("contextmenu", function (e) {
    e.preventDefault();
    menu.style.display = "block";
    const menuWidth = menu.offsetWidth;
    const menuHeight = menu.offsetHeight;
    let x = e.clientX;
    let y = e.clientY;
    if (x + menuWidth > window.innerWidth) {
        x = window.innerWidth - menuWidth - 8;
    }
    if (y + menuHeight > window.innerHeight) {
        y = window.innerHeight - menuHeight - 8;
    }
    menu.style.left = x + "px";
    menu.style.top = y + "px";
});

document.addEventListener("click", function () {
    menu.style.display = "none";
});