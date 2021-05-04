function postParam(path, name, val) {
    const formData = new FormData();
    formData.append(name, val);

    const options = {
        method: 'POST',
        body: formData
    }

    fetch(path, options);
}
