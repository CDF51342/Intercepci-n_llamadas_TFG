function action_color() {
    document.getElementById('box').style.display = 'block';
    document.getElementById('action_button').style.display = 'none';
    document.getElementById('deactivate_button').style.display = 'block';
}

function deactivate_color() {
    document.getElementById('box').style.display = 'none';
    document.getElementById('action_button').style.display = 'block';
    document.getElementById('deactivate_button').style.display = 'none';
}
