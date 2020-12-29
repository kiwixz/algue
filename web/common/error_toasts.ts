import Vue from "vue";
import Toast from "vue-toastification";

import "vue-toastification/dist/index.css";

Vue.use(Toast, {
  closeOnClick: false,
  draggable: false,
  timeout: 10000,
});

Vue.config.errorHandler = function (error, vm, info) {
  console.error("unhandled vue error", { error, vm, info });
  Vue.$toast.error(error.message);
};

window.onerror = (message, source, lineno, colno, error) => {
  console.error("unhandled error", { message, source, lineno, colno, error });
  Vue.$toast.error(message);
};
