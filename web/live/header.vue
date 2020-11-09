<template>
  <div class="header">header (summoner is {{ summoner_name }})</div>
</template>

<style lang="scss">
  .header {
    padding: 10px;

    background-color: #054;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  @Component
  export default class extends Vue {
    @Prop() readonly summoner_name: string | undefined;
  }
</script>
