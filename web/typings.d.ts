declare module "*.vue" {
  import Vue from "vue";
  export default Vue;
}

declare module "*.png" {
  const path: string;
  export default path;
}
